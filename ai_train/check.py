import sys
import os

current_dir = os.path.dirname(os.path.abspath(__file__))
debug_path = os.path.join(current_dir, "Debug")
sys.path.append(debug_path)

try:
    import nw_engine
    print("Core module found!")
    nw_engine.AIEnv.initContext()
    env = nw_engine.AIEnv()
    print("AI Environment initialized successfully!")
except ImportError as e:
    print(f"Failed to load module: {e}")
except Exception as e:
    print(f"Error: {e}")
