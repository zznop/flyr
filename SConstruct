base_env = Environment(
    SRCDIR = 'src',
    BUILDROOT = 'build',
    CC = 'gcc',
    CCFLAGS = [
        '-Wall',
        '-Wextra',
        '-Werror',
        '-std=c99',
    ],
)

envs = []

debug_env = base_env.Clone()
debug_env.Append(
    MODE = 'debug',
    CCFLAGS = [
        '-ggdb3',
    ],
)

envs.append(debug_env)

release_env = base_env.Clone()
release_env.Append(
    CCFLAGS = [
        '-O2',
    ],
    MODE = 'release',
)

envs.append(release_env)

for env in envs:
    flyr = SConscript(
        './src/core/SConscript',
        variant_dir=env['BUILDROOT'] + "/" + env["MODE"] + "/core",
        duplicate=False,
        exports='env',
    )

    flyr_crash = SConscript(
        './src/crash-harnesses/linux/x86-64/SConscript',
        variant_dir=env['BUILDROOT'] + "/" + env["MODE"] + "/crash",
        duplicate=False,
        exports='env',
    )
