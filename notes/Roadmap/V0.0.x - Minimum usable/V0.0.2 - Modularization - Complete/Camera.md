Very simple object to hold a projection matrix and transform editable from elsewhere

> Ended up not being necessary in v0.0.2 because it's just a matrix pushed to the material buffer. This can be handled wherever the material object happens to be; so "extracting" the camera from Quartz::Renderer is free.

- [ ] Initialize matricies
- [ ] Update matricies
	- Don't worry about only updating when dirty or whatever. Will get to that eventually
- [ ] Push data to some camera/global uniform buffer


