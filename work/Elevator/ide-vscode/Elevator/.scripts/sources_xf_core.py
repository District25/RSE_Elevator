import os

Import("env")

print("Calling 'XF core' source import script...")

env.BuildSources(
    variant_dir=os.path.join("$BUILD_DIR", "external", "build", "xf-core"),
    src_dir=os.path.join("$PROJECT_DIR", "../../src/xf/core"),
)
