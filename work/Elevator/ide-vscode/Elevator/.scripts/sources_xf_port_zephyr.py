import os

Import("env")

print("Calling 'XF port zephyr' source import script...")

env.BuildSources(
    variant_dir=os.path.join("$BUILD_DIR", "external", "build", "xf-port-zephyr"),
    src_dir=os.path.join("$PROJECT_DIR", "../../src/xf/port/zephyr"),
)
