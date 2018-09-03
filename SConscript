Import('env')

sources = [
    '$SRCDIR/main.c',
    '$SRCDIR/utils.c',
    '$SRCDIR/controller.c',
    '$SRCDIR/parson/parson.c',
]

dudley = env.Program(
    'dudley',
    source = sources,
)

Return('dudley')
