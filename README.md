# xspeedhack

Installation: `pip install xspeedhack`

Usage:

```python
import xspeedhack as xsh

c = xsh.Client("Celeste.exe", arch="x86") # Select a process by its name, and specify the architecture
c = xsh.Client(process_id=4632, arch="x64") # Or select the process by its id
c.set_speed(0.1)
c.set_speed(10.)
```