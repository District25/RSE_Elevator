import os

Import("env")

print("Calling 'mdw' source import script...")

env.BuildSources(
    variant_dir=os.path.join("$BUILD_DIR", "external", "build", "mdw"),
    src_dir=os.path.join("$PROJECT_DIR", "../../src/mdw")
)
