import os

Import("env")

print("Calling 'platform' source import script...")

env.BuildSources(
    variant_dir=os.path.join("$BUILD_DIR", "external", "build", "platform"),
    src_dir=os.path.join("$PROJECT_DIR", "../../src/platform/nucleo-l476rg-zephyr"),
)
