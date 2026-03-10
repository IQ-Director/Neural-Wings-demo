import json
import random

# 配置贴图路径
RANDOM_TEXTURE_DIR = "assets/textures/random/"
GIF_TEXTURE_PATH = "assets/textures/gif/1.gif"


def rand_vec3(min_val, max_val):
    return [round(random.uniform(min_val, max_val), 2) for _ in range(3)]


def rand_color():
    # 赛博高饱和色池
    colors = [
        [0, 255, 255, 255],   # 青
        [255, 0, 255, 255],   # 洋红
        [255, 255, 0, 255],   # 亮黄
        [0, 255, 128, 255],   # 霓虹绿
        [180, 150, 255, 255]  # 淡紫
    ]
    return random.choice(colors)


def get_random_texture_config():
    """
    随机决定使用 0-10.png 中的一张，或者使用 GIF
    """
    if random.random() > 0.2:
        tex_id = random.randint(1, 7)
        return [f"{RANDOM_TEXTURE_DIR}{tex_id}.png", 1]  # 1 表示静态
    else:
        return [GIF_TEXTURE_PATH, random.randint(5, 15)]  # 随机动画速度


def get_render_config(fs_path, emissive_color, intensity):
    tex_info = get_random_texture_config()
    return {
        "renderScale": [1, 1, 1],
        "isVisible": True,
        "defaultMaterial": {
            "fs": "assets/shaders/default.fs"
        },
        "meshPasses": [
            {
                "meshIndex": 0,
                "name": "Mesh_0",
                "passes": [
                    {
                        "name": "main_pass",
                        "textures": {
                            "u_diffuseMap": tex_info
                        },
                        "vs": "assets/shaders/default.vs",
                        "fs": fs_path,
                        "blendMode": "NONE",
                        "depthWrite": True,
                        "depthTest": True,
                        "emissive": {
                            "color": emissive_color[:3],
                            "intensity": intensity
                        }
                    }
                ]
            }
        ]
    }


def generate_scene():
    scene_range = 1000  # 扩大范围增加宏大感

    scene = {
        "name": "Cyber_Glitch_Void",
        "physics": {
            "physicsStage": {
                "CollisionStage": {"enable": True},
                "SolarStage": {"G": 0.15, "enable": True},  # 开启微弱的星体引力
                "NetworkVerifyStage": {"enable": False}
            }
        },
        "skybox": {
            "texture": "assets/textures/skybox_1.hdr",
            "tint": [255, 255, 255, 255]
        },
        "objectsPools": [
            {
                "name": "bullet",
                "tag": "bullet",
                "prefab": "assets/prefabs/bullet.json",
                "count": 1
            },
            {
                "name": "missile",
                "tag": "bullet",
                "prefab": "assets/prefabs/tracking_bullet.json",
                "count": 1
            },
            {
                "name": "mine",
                "tag": "mine",
                "prefab": "assets/prefabs/mine_bullet.json",
                "count": 1
            }
        ],
        "entities": []
    }

    # 1. 玩家 (Player)
    scene["entities"].append({
        "name": "player_fighter",
        "tag": "cube",
        "prefab": "assets/prefabs/plane.json",
        "position": [0, 50, 0],
        "rotation": [0, -90, 0],
        "physics": {
            "mass": 100,
            "elasticity": 1.0,
            "velocity": [
                0,
                0,
                0
            ]
        },
        "scripts": [
            {"PlayerControlScript": {"Thrust": 4000,
                                     "LiftCoefficient": 0.01,
                                     "DragCoefficient": 0.1,
                                     "PitchPower": 300,
                                     "YawPower": 200,
                                     "RollPower": 200,
                                     "ZoomSpeed": 0.1,
                                     "MinCamDist": 0.1,
                                     "MaxCamDist": 20,
                                     "AlignmentStrength": 200,
                                     "AlignmentTheta": 45,
                                     "AlignmentDamping": 0.5}},
            {"WeaponScript": {"velocity_0": 300,
                              "velocity_1": 30,
                              "fireRate_0": 0.05,
                              "fireRate_1": 0.3,
                              "fireRate_2": 0.5}},
            {
                "CollisionListener": {}
            },
            {
                "RayScript": {}
            },
        ]
    })

    # 2. 随机石膏像 (David Statues)
    for i in range(5):
        color = rand_color()
        scene["entities"].append({
            "name": f"statue_david_{i}",
            "tag": "obstacle",
            "prefab": "assets/prefabs/david.json",
            "position": rand_vec3(-scene_range, scene_range),
            "rotation": rand_vec3(0, 360),
            "scale": [random.uniform(10, 100)] * 3,
            "render": get_render_config("assets/shaders/lighting/lighting.fs", color, random.uniform(0.3, 1.5)),
            "scripts": [{"HealthScript": {"maxHP": 2000,  "flashDuration": 0.1}},
                        {"RotatorScript": {
                            "angluarVelocity": [
                                random.uniform(-10, 10),
                                random.uniform(-10, 10),
                                random.uniform(-10, 10)
                            ]}
                         }
                        ]
        })

    # 3. 巨大赛博星体 (Planets)
    for i in range(20):
        color = rand_color()
        scale = random.uniform(40, 400)
        scene["entities"].append({
            "name": f"cyber_planet_{i}",
            "tag": "planet",
            "prefab": "assets/prefabs/light_sphere.json",
            "position": rand_vec3(-scene_range, scene_range),
            "scale": [scale] * 3,
            "physics": {"mass": scale * 10, "velocity": rand_vec3(-5, 5)},
            "render": get_render_config("assets/shaders/lighting/lighting.fs", color, random.uniform(1.0, 4.0)),
            "scripts": [{"RotatorScript": {"angluarVelocity": rand_vec3(-0.1, 0.1)}}]
        })

    # 4. 随机干扰碎片 (Glitch Cubes)
    for i in range(20):
        color = rand_color()
        scene["entities"].append({
            "name": f"debris_{i}",
            "tag": "debris",
            "prefab": "assets/prefabs/light_cube.json",
            "position": rand_vec3(-scene_range, scene_range),
            "scale": [random.uniform(1, 50)] * 3,
            "render": get_render_config("assets/shaders/lighting/lighting.fs", color, 2.0),
            "physics": {
                "mass": 10,
                "elasticity": 1.0,
                "velocity": [
                    -1,
                    0,
                    0
                ],
                "collidable": False
            },
            "scripts": [{"RotatorScript": {"angluarVelocity": rand_vec3(-2, 2)}}]
        })

    # 输出 JSON
    with open('assets/scenes/cyber_glitch_world.json', 'w', encoding='utf-8') as f:
        json.dump(scene, f, indent=2)
    print("Done! Generated assets/scenes/cyber_glitch_world.json")


if __name__ == "__main__":
    generate_scene()
