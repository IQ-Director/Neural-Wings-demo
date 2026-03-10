import math
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
        [255, 80, 0, 255],      # 熔岩橙 (Lava Orange)
        [255, 0, 60, 255],      # 霓虹猩红 (Neon Scarlet)
        [255, 160, 0, 255],     # 能量金 (Power Gold)
        [255, 220, 0, 255],     # 硫磺黄 (Sulfur Yellow)

    ]
    return random.choice(colors)


def rand_david_color():
    colors = [
        [0, 255, 255, 255],     # 激光青 (Laser Cyan)
        [127, 255, 212, 255],   # 电子绿松石 (Electric Turquoise)
        [0, 255, 150, 255],     # 薄荷荧光 (Cyber Mint)
        [170, 255, 255, 255],   # 冰晶白 (Ice Blue White)
    ]
    return random.choice(colors)


def rand_frag_color():
    colors = [
        [255, 255, 255, 255],
        [255, 0, 255, 255],     # 纯洋红 (Pure Magenta)
        [200, 0, 255, 255],     # 电子紫罗兰 (Electric Violet)
        [255, 0, 127, 255],     # 炫目粉 (Dazzling Rose)
        [130, 0, 255, 255],     # 幽灵紫 (Phantom Purple)
    ]
    return random.choice(colors)


def get_random_texture_config():
    """
    随机决定使用 0-10.png 中的一张，或者使用 GIF
    """
    if random.random() > 0.3:
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


def rand_scale_u_shape(min_val=1, max_val=800):
    theta = random.random() * math.pi
    t = (math.cos(theta) * -0.5) + 0.5

    return round(min_val + t * (max_val - min_val), 2)


def generate_scene():
    scene_range = 3000  # 扩大范围增加宏大感

    scene = {
        "name": "Cyber_Glitch_Void",
        "physics": {
            "physicsStage": {
                "CollisionStage": {"enable": True},
                "SolarStage": {"G": 1.05, "enable": True},  # 开启微弱的星体引力
                "NetworkVerifyStage": {"enable": False}
            }
        },
        "skybox": {
            "texture": "assets/textures/skybox_4.jpeg",
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
        "name": "plane",
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
            {"PlayerControlScript": {"Thrust": 10000,
                                     "LiftCoefficient": 0.00,
                                     "DragCoefficient": 0.1,
                                     "PitchPower": 300,
                                     "YawPower": 200,
                                     "RollPower": 200,
                                     "ZoomSpeed": 0.1,
                                     "MinCamDist": 0.01,
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
            {
                "LocalPlayerSyncScript": {
                    "netObjectID": 1
                }
            }
        ],
        "light": {
            "type": "POINT",
                    "color": [255, 255, 0],
            "intensity": 0.9,
            "range": 120.0,
            "attenuation": 1.0,
            "shadows": True,
            "shadowBias": 0.01
        }
    })

    # # 2. 随机石膏像 (David Statues)
    # for i in range(5):
    #     color = rand_color()
    #     if (random.random() > 0.3):
    #         scene["entities"].append({
    #             "name": f"statue_david_{i}",
    #             "tag": "obstacle",
    #             "prefab": "assets/prefabs/david.json",
    #             "position": rand_vec3(-scene_range, scene_range),
    #             "rotation": rand_vec3(0, 360),
    #             "scale": [random.uniform(10, 300)] * 3,
    #             "render": get_render_config("assets/shaders/lighting/lighting.fs", color, random.uniform(0.6, 1.5)),
    #             "scripts": [{"HealthScript": {"maxHP": 2000,  "flashDuration": 0.1}},
    #                         {"RotatorScript": {
    #                             "angluarVelocity": [
    #                                 rand_vec3(-0.01, 0.01)
    #                             ]}
    #                          }
    #                         ]
    #         })
    #     else:
    #         scene["entities"].append({
    #             "name": f"statue_david_{i}",
    #             "tag": "obstacle",
    #             "prefab": "assets/prefabs/david.json",
    #             "position": rand_vec3(-scene_range, scene_range),
    #             "rotation": rand_vec3(0, 360),
    #             "scale": [random.uniform(10, 300)] * 3,
    #             "render": get_render_config("assets/shaders/lighting/lighting.fs", color, random.uniform(0.3, 1.5)),
    #             "scripts": [{"HealthScript": {"maxHP": 2000,  "flashDuration": 0.1}},
    #                         {"RotatorScript": {
    #                             "angluarVelocity": [
    #                                 rand_vec3(-0.1, 0.1)
    #                             ]}
    #                          }
    #                         ],
    #             "light": {
    #                 "type": "POINT",
    #                 "color": color[:3],
    #                 "direction": [
    #                     0,
    #                     0,
    #                     1
    #                 ],
    #                 "intensity": 0.9,
    #                 "range": 120.0,
    #                 "attenuation": 1.0,
    #                 "shadows": True,
    #                 "shadowBias": 0.01
    #             }
    #         }
    #         )

    # 3. 巨大赛博星体 (Planets)
    for i in range(60):
        color = rand_color()
        scale = rand_scale_u_shape(10, 800)
        scene["entities"].append({
            "name": f"cyber_planet_{i}",
            "tag": "planet",
            "prefab": "assets/prefabs/light_sphere.json",
            "position": rand_vec3(-scene_range, scene_range),
            "scale": [scale] * 3,
            "physics": {"mass": scale * 10, "velocity": rand_vec3(-500, 500)},
            "render": get_render_config("assets/shaders/lighting/lighting.fs", color, random.uniform(1.0, 4.0)),
            "scripts": [{"RotatorScript": {"angluarVelocity": rand_vec3(-10, 10)}}]
        })

    # 4. 随机干扰碎片 (Glitch Cubes)
    for i in range(60):
        color = rand_frag_color()
        scene["entities"].append({
            "name": f"debris_{i}",
            "tag": "debris",
            "prefab": "assets/prefabs/light_cube.json",
            "position": rand_vec3(-scene_range, scene_range),
            "scale": [rand_scale_u_shape(1, 800)] * 3,
            "render": get_render_config("assets/shaders/lighting/lighting.fs", color, random.uniform(0.0, 4.2)),
            "physics": {
                "mass": 10,
                "elasticity": 1.0,
                "velocity": [
                   rand_vec3(-50, 50)
                ],
                "collidable": False
            },
            "scripts": [{"RotatorScript": {"angluarVelocity": rand_vec3(-2, 2)}}]
        })

    for i in range(3):
        color = rand_david_color()
        scene["entities"].append({
            "name": f"debris_{i}",
            "tag": "debris",
            "prefab": "assets/prefabs/david.json",
            "position": rand_vec3(-scene_range, scene_range),
            "scale": [random.uniform(400, 500)] * 3,
            "render": get_render_config("assets/shaders/lighting/lighting.fs", color, random.uniform(1.0, 2.0)),
            "physics": {
                "mass": 10,
                "elasticity": 1.0,
                "velocity": [
                   rand_vec3(-10, 10)
                ],
                "collidable": False
            },
            "scripts": [{"RotatorScript": {"angluarVelocity": rand_vec3(-0.1, 0.1)}}],
            "light": {
                "type": "POINT",
                "color": color[:3],
                "intensity": 0.9,
                "range": 120.0,
                "attenuation": 1.0,
                "shadows": False,
                "shadowBias": 0.01
            }
        })

    # scene["entities"].append({
    #     "name": "dirLight",
    #     "tag": "dirLight",
    #     "prefab": "assets/prefabs/dirLight.json",
    #     "rotation": [
    #         0,
    #         0,
    #         0
    #     ],
    #     "light": {
    #         "type": "DIRECTIONAL",
    #         "color": [
    #             255,
    #             0,
    #             0
    #         ],
    #         "direction": [
    #             1,
    #             -2,
    #             1
    #         ],
    #         "intensity": 1.0,
    #         "range": 500.0,
    #         "attenuation": 1.0,
    #         "shadows": True,
    #         "shadowBias": 0.0001
    #     }
    # })
    # 输出 JSON
    with open('assets/scenes/test_scene.json', 'w', encoding='utf-8') as f:
        json.dump(scene, f, indent=2)
    print("Done! Generated assets/scenes/cyber_glitch_world.json")


if __name__ == "__main__":
    generate_scene()
