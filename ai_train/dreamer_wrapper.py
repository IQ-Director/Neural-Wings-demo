import numpy as np
import embodied
import elements
import os
import sys

# 导入你的 C++ 模块
try:
    import nw_engine
except ImportError:
    # 自动处理路径（如果模块在 Release 文件夹下）
    current_dir = os.path.dirname(os.path.abspath(__file__))
    sys.path.append(os.path.join(current_dir, "Release"))
    import nw_engine


class NWEngineDreamer(embodied.Env):
    def __init__(self, width=64, height=64):
        self._width = width
        self._height = height

        # 1. 初始化 GPU 上下文 (隐藏窗口)
        # 注意：在多进程训练中，initContext 只能在子进程或确保唯一时调用
        nw_engine.AIEnv.initContext(width, height)

        # 2. 实例化环境
        self._env = nw_engine.AIEnv(width, height)
        self._env.init()  # 执行 C++ 中的 Init() 逻辑

        self._done = True

        # 3. 定义观测空间
        # DreamerV3 的 CNN 编码器默认处理 uint8 的 3通道图像 [H, W, 3]
        self._obs_space = {
            'image': elements.Space(np.uint8, (height, width, 3)),
            'reward': elements.Space(np.float32),
            'is_first': elements.Space(bool),
            'is_last': elements.Space(bool),
            'is_terminal': elements.Space(bool),
        }

        self._act_space = {
            # 0,1,2 -> 映射为 -1,0,1
            'pitch':   elements.Space(np.int32, (), 0, 3),
            'yaw':     elements.Space(np.int32, (), 0, 3),  # 0,1,2
            'roll':    elements.Space(np.int32, (), 0, 3),  # 0,1,2
            'forward': elements.Space(np.int32, (), 0, 2),  # 0,1
            'back':    elements.Space(np.int32, (), 0, 2),  # 0,1
            'fire':    elements.Space(np.int32, (), 0, 2),  # 0,1
            'reset':   elements.Space(bool),
        }

    @property
    def obs_space(self): return self._obs_space

    @property
    def act_space(self): return self._act_space

    def reset(self):
        self._done = False
        raw_obs = self._env.reset()
        return self._obs(raw_obs, 0.0, is_first=True)

    def step(self, action):
        if action['reset'] or self._done:
            return self.reset()

        processed_action = [
            float(action['pitch'] - 1),   # 0,1,2 -> -1.0, 0.0, 1.0
            float(action['yaw'] - 1),     # 0,1,2 -> -1.0, 0.0, 1.0
            float(action['roll'] - 1),    # 0,1,2 -> -1.0, 0.0, 1.0
            float(action['forward']),     # 0,1
            float(action['back']),        # 0,1
            float(action['fire'])         # 0,1
        ]

        raw_obs, reward, done = self._env.step(processed_action)

        self._done = done
        return self._obs(raw_obs, reward, is_last=done, is_terminal=done)

    def _obs(self, raw_obs, reward, is_first=False, is_last=False, is_terminal=False):
        rgb = (raw_obs[:, :, :3] * 255.0).clip(0, 255).astype(np.uint8)
        image_uint8 = (rgb * 255.0).clip(0, 255).astype(np.uint8)

        return dict(
            image=image_uint8,
            reward=np.float32(reward),
            is_first=is_first,
            is_last=is_last,
            is_terminal=is_terminal,
        )
