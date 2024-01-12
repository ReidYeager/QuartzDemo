Define a clean way to interact with the engine from the client game
Should allow usage of all modules with public functionality

I don't particularly like this method of overriding an "Application" class:
```c++
class Application
{
public:
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Shutdown() = 0;
}

// Game.cpp
class GameApp : public Application
{
public:
	void Init() override;
	void Update() override;
	void Shutdown() override;
}

Application* QuartzCreateApplication()
{
	return new GameApp();
}
```
This feels really limiting for some reason.
Might have to end up using it

---
Ended up implementing the game as a layer with defines for an entrypoint and game-layer retrieval
Not amazing but it'll work for now
