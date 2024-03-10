# Flow
## Init
- client `Quartz::Run()`
- Window
	- > Required for the renderer to initialize
- Renderer
	- Backend api
- Object pools
- Ecs
- Layers
	- > Non-client
- Client layer
- Clocks

## Update

# Layer stack
- > Stack top
- Debug ui (Imgui)
	- > Including the client's
- Ui
- Debug meshes
	- > Once implemented
	- Gizmos
- Input handling
	- > Translating inputs into `Quartz::Input` and events
- Client

# Long term
- Load the client as a hot-swappable dll

