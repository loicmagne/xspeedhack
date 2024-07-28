import time
import struct
import platform
import psutil
import subprocess
from pathlib import Path
from typing import Literal
from importlib import resources

if platform.system() == "Windows":
    import win32file


def resolve_resource(resource_name: str) -> Path:
    return Path(resources.files("xspeedhack.bin") / resource_name)


RESOURCES = {
    "x86": {
        "dll": resolve_resource("speedhack32.dll"),
        "inj": resolve_resource("injector32.exe"),
    },
    "x64": {
        "dll": resolve_resource("speedhack64.dll"),
        "inj": resolve_resource("injector64.exe"),
    },
}


def get_pid(process_name: str) -> int:
    for proc in psutil.process_iter():
        if proc.name() == process_name:
            return proc.pid
    raise RuntimeError(f"Process {process_name} not open")


class SpeedHackClient:
    PIPE_NAME = r"\\.\pipe\xspeedhackpipe"

    def __init__(
        self,
        process_name: str = None,
        process_id: int = None,
        arch: Literal["x86", "x64"] = "x64",
    ):
        """
        Args:
            process_name: Name of the process to connect to. If provided, process_id is ignored.
            process_id: ID of the process to connect to. If provided, process_name is ignored.
            arch: Architecture of the process to connect to. "x86" for 32-bit processes and "x64" for 64-bit processes.
        """
        assert (
            process_name is not None or process_id is not None
        ), "Either process_name or process_id must be provided"

        if process_name is not None:
            self.pid = get_pid(process_name)
        else:
            self.pid = process_id

        self.pipe = None
        self.arch = arch

        self.inject_dll(self.pid)
        time.sleep(
            1
        )  # Give the pipe time to come up after the injection (very conservative)
        self.pipe = self._connect_pipe()

    def set_speed(self, value: float):
        assert value >= 0
        win32file.WriteFile(self.pipe, struct.pack("f", value))

    def _connect_pipe(self) -> int:
        return win32file.CreateFile(
            self.PIPE_NAME,
            win32file.GENERIC_WRITE,
            0,
            None,
            win32file.OPEN_EXISTING,
            0,
            None,
        )

    def __del__(self):
        """Close the pipe handle on deletion."""
        if self.pipe is not None:
            try:
                win32file.CloseHandle(self.pipe)
            except (
                TypeError
            ):  # Throws NoneType object not callable error for some reason
                ...

    def inject_dll(self, pid: int):
        path_dll = RESOURCES[self.arch]["dll"]
        path_injector = RESOURCES[self.arch]["inj"]
        subprocess.run([path_injector, str(pid), str(path_dll)], check=True)
