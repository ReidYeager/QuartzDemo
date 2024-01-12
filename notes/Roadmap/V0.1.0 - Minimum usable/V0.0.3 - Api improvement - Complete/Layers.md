A stack that defines the order in which draw events are called and input is handled

Rendering occurs bottom-up
- Allows higher layers to render on top of lower layers
- Also means sorting and partitioning doesn't need to deal with exceptions for UI, debug, etc.
Event handling occurs top-down
- This allows layers to "consume" the input to prevent later layers from also triggering functionality from it
	- ex: UI consumes click preventing game layer from firing weapon

