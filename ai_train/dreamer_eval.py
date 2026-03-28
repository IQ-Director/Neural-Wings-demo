from pathlib import Path
import cv2
import shutil
import elements
import numpy as np
import embodied
from functools import partial as bind
import ruamel.yaml as yaml
from dreamer_wrapper import NWEngineDreamer
import jax
import os

# 1. 严格在导入 jax 之前锁定环境变量
os.environ['JAX_PLATFORMS'] = 'cuda'
os.environ['JAX_PLATFORM_NAME'] = 'cuda'
os.environ['SDL_AUDIODRIVER'] = 'dummy'
os.environ['ALSOFT_DRIVERS'] = 'null'

os.environ['XLA_FLAGS'] = (
    '--xla_gpu_strict_conv_algorithm_picker=false '
    '--xla_gpu_force_compilation_parallelism=1'
)
os.environ['XLA_PYTHON_CLIENT_PREALLOCATE'] = 'false'
os.environ['XLA_PYTHON_CLIENT_MEM_FRACTION'] = '0.40'

print("Available devices:", jax.devices())


current_dir = Path(__file__).parent.resolve()


def make_env(config, index=0):
    env = NWEngineDreamer(width=64, height=64)
    env = embodied.wrappers.TimeLimit(env, 1000)
    env = embodied.wrappers.UnifyDtypes(env)
    env = embodied.wrappers.CheckSpaces(env)
    return env


def make_agent_dummy(config):
    from dreamerv3.agent import Agent
    obs_space = {
        'image': elements.Space(np.uint8, (64, 128, 3)),
        'reward': elements.Space(np.float32),
        'is_first': elements.Space(bool),
        'is_last': elements.Space(bool),
        'is_terminal': elements.Space(bool),
    }
    act_space = {
        'pitch': elements.Space(np.int32, (), 0, 3),
        'yaw': elements.Space(np.int32, (), 0, 3),
        'roll': elements.Space(np.int32, (), 0, 3),
        'forward': elements.Space(np.int32, (), 0, 2),
        'back': elements.Space(np.int32, (), 0, 2),
        'fire': elements.Space(np.int32, (), 0, 2),
    }
    agent_config = elements.Config(
        **config.agent,
        logdir=config.logdir,
        seed=config.seed,
        jax=config.jax,
        batch_size=config.batch_size,
        batch_length=config.batch_length,
        replay_context=config.replay_context,
        report_length=config.report_length,
    )
    return Agent(obs_space, act_space, agent_config)


def main():
    # ---------- 配置与路径准备 ----------
    configs_path = current_dir / 'configs.yaml'
    configs = yaml.YAML(typ='safe').load(Path(configs_path).read_text())
    config = elements.Config(configs['defaults'])
    config = config.update(configs['size1m'])

    new_logdir_pattern = current_dir / 'logdir' / 'eval_run' / '{timestamp}'
    config = config.update({
        'logdir': str(new_logdir_pattern),
        'run.train_ratio': 32,
        'batch_size': 4,
        'batch_length': 32,
        'jax.platform': 'gpu',
        'jax.profiler': False,
    })

    logdir = elements.Path(config.logdir.format(
        timestamp=elements.timestamp()))
    config = config.update(logdir=str(logdir))
    logdir.mkdir()

    old_folder_name = '20260326T211530'
    source_ckpt_dir = current_dir / 'logdir' / \
        'nw_engine_v1' / old_folder_name / 'checkpoint.pkl'

    latest_file = source_ckpt_dir / 'latest'

    if not latest_file.exists():
        return
    latest_sub_dir = latest_file.read_text().strip()
    target_ckpt_dir = source_ckpt_dir / latest_sub_dir

    agent = make_agent_dummy(config)

    cp = elements.Checkpoint()
    cp.agent = agent
    cp.load(str(target_ckpt_dir), keys=['agent'])
    print("Agent weights loaded successfully!")

    fns = [bind(make_env, config, 0)]
    driver = embodied.Driver(fns, parallel=False)

    quit_flag = False

    def render_cv2(tran, worker):
        nonlocal quit_flag
        image = tran['image']
        depth = tran['log/depth']
        canvas_rgb = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
        canvas_rgb = cv2.resize(canvas_rgb, (512, 512),
                                interpolation=cv2.INTER_NEAREST)

        cv2.putText(canvas_rgb, f"Reward: {tran['reward']:.4f}", (20, 40),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 255, 0), 2)
        canvas_depth = cv2.cvtColor(depth, cv2.COLOR_GRAY2BGR)
        canvas_depth = cv2.resize(
            canvas_depth, (512, 512), interpolation=cv2.INTER_NEAREST)
        cv2.putText(canvas_depth, "Depth Map", (20, 40),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 255, 255), 2)

        combined = np.hstack((canvas_rgb, canvas_depth))

        cv2.imshow("DreamerV3 AI View (RGB + Depth)", combined)

        if tran['is_last'] or tran['is_terminal']:
            print(f"Episode Done. Last Reward: {tran['reward']:.4f}")

        if cv2.waitKey(1) & 0xFF == ord('q'):
            quit_flag = True

    driver.on_step(render_cv2)

    print("Evaluation loop started. Press 'q' to quit.")

    driver.reset(agent.init_policy)
    policy = lambda *args: agent.policy(*args, mode='eval')

    while not quit_flag:
        driver(policy, steps=1)

    cv2.destroyAllWindows()


if __name__ == '__main__':
    main()
