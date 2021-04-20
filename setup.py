from cx_Freeze import setup, Executable

setup(
    name="opticpy",
    version="0.1",
    description="Optic-Streamer Python Client (Compiled)",
    executables=[Executable("opticpy.py")],
    options={
        "build_exe": {"excludes": ["tkinter"]},
    },
)