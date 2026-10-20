def can_build(env, platform):
    return True


def configure(env):
    pass


def get_doc_classes():
    return [
        "UUID",
        "DictUtils",
        "FileUtils",
        "DirUtils",
        "Prefab",
        "ProjectSetting",
        "ProjectSettingsManager",
        "ClassDocReader",
        "JsonSchemaValidator"
    ]


def get_doc_path():
    return "doc/classes"