
import os
from dreamer_wrapper import NWEngineDreamer
import ruamel.yaml as yaml
from functools import partial as bind
import embodied
import numpy as np
import elements
import pathlib
import shutil

import sys
import jax
from pathlib import Path

os.environ['SDL_AUDIODRIVER'] = 'dummy'
os.environ['ALSOFT_DRIVERS'] = 'null'

os.environ['XLA_FLAGS'] = (
    '--xla_gpu_strict_conv_algorithm_picker=false '
    '--xla_gpu_force_compilation_parallelism=1'
)
os.environ['XLA_PYTHON_CLIENT_PREALLOCATE'] = 'false'
os.environ['XLA_PYTHON_CLIENT_MEM_FRACTION'] = '0.40'
os.environ['jax_platforms'] = 'cuda'
# root = pathlib.Path(__file__).parent
# sys.path.insert(0, str(root))

print("Available devices:", jax.devices())

current_dir = Path(__file__).parent.resolve()


def main():
    configs_path = current_dir / 'configs.yaml'
    configs = yaml.YAML(typ='safe').load(Path(configs_path).read_text())

    config = elements.Config(configs['defaults'])
    config = config.update(configs['size1m'])

    old_folder_name = '20260328T152324'
    old_logdir = current_dir / 'logdir' / 'nw_engine_v1_depth' / old_folder_name

    new_logdir_pattern = current_dir / \
        'logdir' / 'nw_engine_v1_depth' / '{timestamp}'

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

    old_checkpoint = elements.Path(old_logdir) / 'checkpoint.pkl'
    if old_checkpoint.exists():
        shutil.copytree(str(old_checkpoint), str(logdir / 'checkpoint.pkl'))

        old_replay = elements.Path(old_logdir) / 'replay'
        if old_replay.exists():
            shutil.copytree(str(old_replay), str(
                logdir / 'replay'), dirs_exist_ok=True)
    else:
        print(f"{old_logdir} NOT found checkpoint.pkl")

    config.save(logdir / 'config.yaml')

    run_args = elements.Config(
        **config.run,
        logdir=config.logdir,
        batch_size=config.batch_size,
        batch_length=config.batch_length,
        report_length=config.report_length,
        consec_train=config.consec_train,
        consec_report=config.consec_report,
        replay_context=config.replay_context,
    )

    embodied.run.train(
        bind(make_agent, config),
        bind(make_replay, config, 'replay'),
        bind(make_env, config),
        bind(make_stream, config),
        bind(make_logger, config),
        run_args
    )


def make_agent(config):
    # from dreamerv3.agent import Agent
    # env = make_env(config)
    # obs_space = {k: v for k, v in env.obs_space.items()
    #              if not k.startswith('log/')}
    # act_space = {k: v for k, v in env.act_space.items() if k != 'reset'}
    # # env.close()

    # return Agent(obs_space, act_space, elements.Config(
    #     **config.agent,
    #     logdir=config.logdir,
    #     seed=config.seed,
    #     jax=config.jax,
    #     batch_size=config.batch_size,
    #     batch_length=config.batch_length,
    #     replay_context=config.replay_context,
    #     report_length=config.report_length,
    # ))
    return make_agent_dummy(config)


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


def make_env(config, index=0, **kwargs):
    env = NWEngineDreamer(width=64, height=64)
    env = embodied.wrappers.TimeLimit(env, 1000)
    env = embodied.wrappers.UnifyDtypes(env)
    env = embodied.wrappers.CheckSpaces(env)
    return env


def make_logger(config):
    step = elements.Counter()
    logdir = elements.Path(config.logdir)
    outputs = [
        elements.logger.TerminalOutput(config.logger.filter),
        elements.logger.JSONLOutput(logdir, 'metrics.jsonl'),
        elements.logger.TensorBoardOutput(logdir, config.logger.fps),
    ]
    return elements.Logger(step, outputs)


def make_replay(config, folder, mode='train'):
    length = config.batch_length + config.replay_context
    capacity = int(config.replay.size)
    directory = elements.Path(config.logdir) / folder
    return embodied.replay.Replay(
        length=length, capacity=capacity, directory=directory,
        online=config.replay.online, chunksize=config.replay.chunksize)


def make_stream(config, replay, mode='train'):
    from dreamerv3.main import make_stream as common_stream
    return common_stream(config, replay, mode)


if __name__ == '__main__':
    # env = NWEngineDreamer(width=64, height=64)
    # obs = env.reset()
    # test_action = {
    #     'pitch': 1,
    #     'yaw': 1,
    #     'roll': 1,
    #     'forward': 0,
    #     'back': 0,
    #     'fire': 0,
    #     'reset': False
    # }
    # try:
    #     for i in range(1, 101):
    #         obs = env.step(test_action)

    #         if i % 20 == 0:
    #             print(
    #                 f" {i}: reward={obs['reward']:.4f}, {obs['is_terminal']}")

    #     print("Env is fine.")

    # except Exception as e:
    # print(f"{e}")
    # configs_path = current_dir / 'configs.yaml'
    # configs = yaml.YAML(typ='safe').load(Path(configs_path).read_text())

    # config = elements.Config(configs['defaults'])
    # config = config.update(configs['size1m'])

    # # 2. 自定义修改参数
    # config = config.update({
    #     'logdir': '~/logdir/nw_engine_v1',
    #     'run.train_ratio': 32,
    #     'batch_size': 4,
    #     'batch_length': 32,
    #     'jax.platform': 'gpu',
    # })

    # # 格式化 logdir 路径
    # logdir = elements.Path(config.logdir.format(
    #     timestamp=elements.timestamp()))
    # config = config.update(logdir=str(logdir))
    # logdir.mkdir()
    # config.save(logdir / 'config.yaml')

    # # 3. 准备运行参数
    # run_args = elements.Config(
    #     **config.run,
    #     logdir=config.logdir,
    #     batch_size=config.batch_size,
    #     batch_length=config.batch_length,
    #     report_length=config.report_length,
    #     consec_train=config.consec_train,
    #     consec_report=config.consec_report,
    #     replay_context=config.replay_context,
    # )

    # # agent = make_agent(config)
    # agent = make_agent_dummy(config)
    # env = make_env(config)
    # replay = make_replay(config, 'replay')
    # logger = make_logger(config)

    # print("开始手动单步调试...")
    # obs = env.reset()
    # policy = agent.init_policy(1)  # 初始化 Agent 状态

    # for i in range(10):
    #     # 这里的 policy 逻辑比较复杂，我们只测最容易崩的 step
    #     action = {
    #         'pitch': np.int32(1),
    #         'yaw': np.int32(1),
    #         'roll': np.int32(1),
    #         'forward': np.int32(0),
    #         'back': np.int32(0),
    #         'fire': np.int32(0),
    #         'reset': np.bool_(False)
    #     }
    #     print(f"Step {i} 尝试中...")
    #     obs = env.step(action)  # 如果崩在这里，是 C++ 引擎问题
    #     print(f"Step {i} 环境成功")

    main()
