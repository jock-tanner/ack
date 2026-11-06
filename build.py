from build.ab import export

# The only platform is the Stackette.
PLATS = ['stackette']

# This contains the platform-independent host tooling required to build the plats.
export(
    name='common',
    deps=[
        'lang/cem/cemcom.ansi+all',
        'lang/cem/cpp.ansi+all',
        'util/ack+all',
        'util/amisc+all',
        'util/arch+all',
        'util/ego+all',
        'util/led+all',
        'util/misc+all',
        'util/opt+all',
    ],
)

export(
    name='compiler',
    deps=(['.+common'] + [f'plat/{p}+all' for p in PLATS]),
)

export(
    name='all',
    deps=['.+compiler', 'lang/cem/cpp.ansi+test'],
)
