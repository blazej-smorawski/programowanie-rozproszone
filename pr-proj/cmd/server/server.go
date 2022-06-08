package main

import (
	"encoding/gob"
	"fmt"
	"log"
	"math"
	"math/rand"
	"net"
	"sync"
	"time"
)

type ServerPlayerData struct {
	Id   int
	X    float32
	Y    float32
	Size float32
}

type PlayerData struct {
	Id   int
	X    float32
	Y    float32
	Size float32
}

type PlayerInput struct {
	Horizontal float32
	Vertical   float32
}

type GameState struct {
	PlayerCount int
	FoodCount   int
	Players     [64]PlayerData
	Food        [64]PlayerData
	Trap        [64]PlayerData
}

func sendPlayerToState(player *ServerPlayerData) {
	state.Players[player.Id].Id = player.Id
	state.Players[player.Id].X = player.X
	state.Players[player.Id].Y = player.Y
}

func handlePlayer(conn net.Conn, id int) {
	fmt.Printf("Connected to: ")
	fmt.Println(conn.RemoteAddr())

	encoder := gob.NewEncoder(conn)
	decoder := gob.NewDecoder(conn)
	var input PlayerInput
	var player ServerPlayerData
	var err error

	// Setup player
	player.Id = id
	player.X = rand.Float32()
	player.Y = rand.Float32()
	state.Players[player.Id].Size = 0.1
	for {
		sendPlayerToState(&player)
		state.Players[player.Id].Id = player.Id
		t := time.Now()
		err = encoder.Encode(state)
		if err != nil {
			goto closePlayer
		}

		err = decoder.Decode(&input)
		if err != nil {
			goto closePlayer
		}
		// Movement
		vmax := 0.02
		VX := float32(math.Max(math.Min(float64(input.Horizontal/10), vmax), -vmax))
		VY := float32(math.Max(math.Min(float64(input.Vertical/10), vmax), -vmax))
		player.X = float32(math.Max(math.Min(float64(player.X+VX), 1), 0))
		player.Y = float32(math.Max(math.Min(float64(player.Y+VY), 1), 0))

		collisionMain <- 1
		ret := <-collisionAll[player.Id]
		if ret == -1 {
			fmt.Println("Collision ret = " + string(ret))
			goto closePlayer
		}

		for time.Now().Sub(t).Nanoseconds() < time.Second.Nanoseconds()/30 {

		}
	}
closePlayer:
	if state.Players[player.Id].Id != -1 {
		state.PlayerCount--
		state.Players[player.Id].Id = -1
		state.Players[player.Id].X = rand.Float32()
		state.Players[player.Id].Y = rand.Float32()
	}
	fmt.Println("Goroutine quit!")
}

func handleFood() {
	for {
		if state.FoodCount < 64 {
			for i := 0; i < 64; i++ {
				if state.Food[i].Id == -1 {
					stateMutex.Lock()
					state.Food[i].Id = 1
					state.Food[i].Size = 0.01
					state.Food[i].X = rand.Float32()
					state.Food[i].Y = rand.Float32()
					stateMutex.Unlock()
					break
				}
			}
			time.Sleep(time.Duration(time.Second.Seconds()*rand.Float64() + 0.5))
		}
	}
}

func addSize(p *PlayerData, s float32) {
	p.Size = float32(math.Sqrt(math.Pow(float64(p.Size), 2) + math.Pow(float64(s), 2)))
}

func collision(p1 *PlayerData, p2 *PlayerData) int {
	R := math.Max(float64(p1.Size), float64(p2.Size))
	d := math.Sqrt(math.Pow(float64(p1.X-p2.X), 2) + math.Pow(float64(p1.Y-p2.Y), 2))
	if R > d {
		if R == float64(p1.Size) {
			return 1
		} else {
			return 2
		}
	}
	return 0
}

func handleCollisions() {
	var addedIds [64]int
	addedPlayers := 0
	for {
		recv := 0
		firstSignal := <-collisionMain
		// Now we must process all collisions without any new players
		stateMutex.Lock()
		recv++
		if firstSignal != 1 {
			break
		}

		for recv != state.PlayerCount {
			select {
			case signal := <-collisionMain:
				recv++
				if signal != 1 {
					break
				}
			default:
				fmt.Printf("\rNo activity")
			}
		}

		addedPlayers = 0
		for i := 0; i < 64; i++ {
			if state.Players[i].Id != -1 {
				for j := 0; j < 64; j++ {
					if state.Food[j].Id != -1 {
						col := collision(&state.Players[i], &state.Food[j])
						if col == 1 {
							addSize(&state.Players[i], state.Food[j].Size)
							state.Food[j].Id = -1
						}
					}
				}
				for j := 0; j < 64; j++ {
					if j != i && state.Players[j].Id != -1 {
						col := collision(&state.Players[i], &state.Players[j])
						if col == 1 {
							addSize(&state.Players[i], state.Players[j].Size)
							state.Players[j].Id = -1
							state.Players[j].X = rand.Float32()
							state.Players[j].Y = rand.Float32()
							state.PlayerCount--
							addedIds[addedPlayers] = j
							addedPlayers++
							fmt.Println("2nd Dead")
						} else if col == 2 {
							addSize(&state.Players[j], state.Players[i].Size)
							state.Players[i].Id = -1
							state.PlayerCount--
							state.Players[i].X = rand.Float32()
							state.Players[i].Y = rand.Float32()
							fmt.Println("First Dead")
							break
						}
					}
				}
				addedIds[addedPlayers] = i
				addedPlayers++
			}
		}

		for i := 0; i < addedPlayers; i++ {
			fmt.Printf("\rAdded[0]=%d, Added[1]=%d", addedIds[0], addedIds[1])
			collisionAll[addedIds[i]] <- state.Players[addedIds[i]].Id
		}
		stateMutex.Unlock()
	}
}

var (
	stateMutex    sync.Mutex
	state         GameState
	collisionMain chan int
	collisionAll  [64]chan int
)

func main() {
	// Setup GameState
	collisionMain = make(chan int)
	for i := range collisionAll {
		collisionAll[i] = make(chan int)
	}

	for i := 0; i < 64; i++ {
		state.Players[i].Id = -1
		state.Food[i].Id = -1
		state.Trap[i].Id = -1
	}
	go handleFood()
	go handleCollisions()

	server, _ := net.Listen("tcp", "127.0.0.1:16777")

	for {
		fmt.Println("Listening...")
		conn, _ := server.Accept()

		fmt.Printf("Connected to: ")
		fmt.Println(conn.RemoteAddr())

		encoder := gob.NewEncoder(conn)
		decoder := gob.NewDecoder(conn)
		var firstMessage string
		var err error
		var player ServerPlayerData
		var newId int = -1

		err = decoder.Decode(&firstMessage)
		if err != nil {
			conn.Close()
		}

		if firstMessage != "Join request" {
			fmt.Println("First message from a new player" +
				"is wrong!")
			conn.Close()
		}

		stateMutex.Lock()
		state.PlayerCount++
		for i := 0; i < 64; i++ {
			if state.Players[i].Id == -1 {
				newId = i
				break
			}
		}
		if newId == -1 {
			err = encoder.Encode(-1)
			conn.Close()
		}

		player.Id = newId
		err = encoder.Encode(player.Id)
		if err != nil {

			conn.Close()
		}
		stateMutex.Unlock()

		go handlePlayer(conn, newId)
	}

	err := server.Close()
	checkErr(err)
}

func checkErr(err error) {
	if err != nil {
		log.Fatal(err)
	}
}
