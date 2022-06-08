package main

import (
	"encoding/gob"
	"fmt"
	"github.com/faiface/pixel"
	"github.com/faiface/pixel/imdraw"
	"github.com/faiface/pixel/pixelgl"
	"golang.org/x/image/colornames"
	"log"
	"math"
	"math/rand"
	"net"
	"time"
)

type PlayerData struct {
	Id   int
	X    float32
	Y    float32
	Size float32
}

type GameState struct {
	PlayerCount int
	FoodCount   int
	Players     [64]PlayerData
	Food        [64]PlayerData
	Trap        [64]PlayerData
}

type PlayerInput struct {
	Horizontal float32
	Vertical   float32
}

func run() {
	var state GameState
	var previousState GameState
	var input PlayerInput
	var lastReceived time.Time

	// Chanel for communication between drawing thread and network thread
	c := make(chan int)
	go handleServerConnection(c, &state, &previousState, &input, &lastReceived)

	// Wait for id
	myId := <-c
	if myId == -1 {
		fmt.Println("Server is full!")
		return
	}

	cfg := pixelgl.WindowConfig{
		Title:  "Game!",
		Bounds: pixel.R(0, 0, 1024, 1024),
		VSync:  true,
	}
	win, err := pixelgl.NewWindow(cfg)
	if err != nil {
		panic(err)
	}

	imd := imdraw.New(nil)
	for !win.Closed() {
		// Draw food
		for i := 0; i < 64; i++ {
			if state.Food[i].Id != -1 {
				imd.Color = colornames.Limegreen
				x := state.Food[i].X
				y := state.Food[i].Y
				imd.Push(pixel.V(float64(x)*win.Bounds().W(), float64(y)*win.Bounds().W()))
				imd.Circle(float64(state.Food[i].Size)*win.Bounds().W(), 0)
			}
		}
		// Iterate over players
		fmt.Printf("ID:%d S:%f - ID:%d S:%f\r", state.Players[0].Id, state.Players[0].Size, state.Players[1].Id, state.Players[1].Size)
		for i := 0; i < 64; i++ {
			// Drawing
			t := float32(float64(-time.Until(lastReceived)) / float64(int(time.Second.Nanoseconds()/30)))
			t = float32(math.Min(float64(t), 1))

			if state.Players[i].Id != -1 {

				imd.Color = randomNiceColor(int64(i))
				x := lerp(previousState.Players[i].X, state.Players[i].X, t)
				y := lerp(previousState.Players[i].Y, state.Players[i].Y, t)
				imd.Push(pixel.V(float64(x)*win.Bounds().W(), float64(y)*win.Bounds().W()))
				imd.Circle(float64(state.Players[i].Size)*win.Bounds().W(), 0)
			}

			// Handle input
			if state.Players[i].Id == myId {
				input.Horizontal = float32(win.MousePosition().X/win.Bounds().W() - float64(state.Players[myId].X))
				input.Vertical = float32(win.MousePosition().Y/win.Bounds().W() - float64(state.Players[myId].Y))
			}
		}
		win.Clear(colornames.Aliceblue)
		imd.Draw(win)
		imd.Clear()
		win.Update()
	}
}

func handleServerConnection(c chan int, state *GameState, previousState *GameState, input *PlayerInput, lastReceived *time.Time) {
	conn, err := net.Dial("tcp", "127.0.0.1:16777")
	checkErr(err)

	encoder := gob.NewEncoder(conn)
	decoder := gob.NewDecoder(conn)

	// Send first message to the server
	err = encoder.Encode("Join request")
	checkErr(err)

	var id int
	err = decoder.Decode(&id)
	checkErr(err)

	// Send id to drawing thread
	c <- id

	err = decoder.Decode(&state)
	*previousState = *state
	err = encoder.Encode(*input)
	*lastReceived = time.Now()

	var tempState GameState
	for {
		tempState = *state
		err = decoder.Decode(&state)
		checkErr(err)
		*previousState = tempState
		*lastReceived = time.Now()
		err = encoder.Encode(*input)
		checkErr(err)
	}
}

func main() {
	pixelgl.Run(run)
}

/*func randomNiceColor(seed int64) pixel.RGBA {
	return pixel.RGB(float64(seed)/64+0.2, float64(seed)/64+0.2, float64(seed)/64+0.2)
}*/

func randomNiceColor(seed int64) pixel.RGBA {
again:
	rand.Seed(seed)
	r := rand.Float64()
	g := rand.Float64()
	b := rand.Float64()
	len := math.Sqrt(r*r + g*g + b*b)
	if len == 0 {
		goto again
	}
	return pixel.RGB(r/len, g/len, b/len)
}

func lerp(v0 float32, v1 float32, t float32) float32 {
	return (1-t)*v0 + t*v1
}

func checkErr(err error) {

	if err != nil {
		log.Fatal(err)
	}
}
