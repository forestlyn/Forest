from pathlib import Path

from PIL import Image, ImageDraw


FRAME_SIZE = 64
SHEET_SIZE = (256, 256)


def _frame_yaml(name, x, y, w, h, trimmed=False, source_x=0, source_y=0):
    trimmed_value = "true" if trimmed else "false"
    return f"""  {name}:
    frame:
      x: {x}
      y: {y}
      w: {w}
      h: {h}
    rotated: false
    trimmed: {trimmed_value}
    spriteSourceSize:
      x: {source_x}
      y: {source_y}
      w: {w}
      h: {h}
    sourceSize:
      w: {FRAME_SIZE}
      h: {FRAME_SIZE}
    pivot:
      x: 0.5
      y: 0.5
"""


def generate_test_spritesheet():
    output_dir = Path(__file__).resolve().parent
    image_path = output_dir.parent / "Textures" / "spritesheet.png"
    yaml_path = output_dir / "spritesheet.yaml"
    image_path.parent.mkdir(parents=True, exist_ok=True)

    image = Image.new("RGBA", SHEET_SIZE, (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)

    # Four 64x64 run frames with distinct colors and frame numbers.
    colors = [
        (231, 76, 60, 255),
        (46, 204, 113, 255),
        (52, 152, 219, 255),
        (241, 196, 15, 255),
    ]

    for i, color in enumerate(colors):
        x = i * FRAME_SIZE
        y = 0
        draw.rectangle([x, y, x + FRAME_SIZE - 1, y + FRAME_SIZE - 1], fill=color)
        draw.text((x + 26, y + 24), str(i), fill=(255, 255, 255, 255))

    # One trimmed jump frame: source size is 64x64, atlas rect is 48x50.
    jump_x = 0
    jump_y = FRAME_SIZE
    draw.rectangle([jump_x, jump_y, jump_x + 47, jump_y + 49], fill=(155, 89, 182, 255))
    draw.text((jump_x + 20, jump_y + 18), "J", fill=(255, 255, 255, 255))

    image.save(image_path)

    frames_yaml = [
        _frame_yaml(f"hero_run_{i}.png", i * FRAME_SIZE, 0, FRAME_SIZE, FRAME_SIZE)
        for i in range(4)
    ]
    frames_yaml.append(
        _frame_yaml("hero_jump.png", 0, FRAME_SIZE, 48, 50, trimmed=True, source_x=8, source_y=7)
    )

    yaml_text = (
        "frames:\n"
        + "".join(frames_yaml)
        + "meta:\n"
        + "  image: ../Textures/spritesheet.png\n"
        + "  format: RGBA8888\n"
        + "  size:\n"
        + f"    w: {SHEET_SIZE[0]}\n"
        + f"    h: {SHEET_SIZE[1]}\n"
        + "  scale: 1\n"
    )
    yaml_path.write_text(yaml_text, encoding="utf-8")

    print(f"Generated test spritesheet: {image_path}")
    print(f"Generated YAML atlas config: {yaml_path}")


if __name__ == "__main__":
    generate_test_spritesheet()
