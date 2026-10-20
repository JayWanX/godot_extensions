#include "file_utils.h"

#include "core/io/file_access.h"
#include "core/object/class_db.h"
#include "core/string/ustring.h"

void FileUtils::_bind_methods() {
	ClassDB::bind_static_method("FileUtils",
			D_METHOD("is_text_file", "file", "max_read_bytes"),
			&FileUtils::is_text_file, DEFVAL(4096));
}

const HashSet<String> &FileUtils::_text_extensions() {
	static const HashSet<String> exts = {
		// Godot 资源/脚本
		"gd", "godot", "tscn", "tres", "cfg", "config", "import", "remap", "gdignore",
		"gdshader", "gdshaderinc", "theme", "cs", "csproj", "sln", "props", "targets",
		// 数据与标记语言
		"json", "gltf", "csv", "tsv", "md", "markdown", "mdown", "txt", "text", "ini",
		"toml", "conf", "xml", "yml", "yaml", "svg", "graphml", "rss", "atom", "opml",
		"dae", "mtl", "x3d", "wrl", "x3dv", "ipynb", "wat", "cson", "hjson",
		"json5", "jsonc", "jsonl", "ndjson", "gql", "graphql",
		// Web
		"html", "htm", "xhtml", "css", "scss", "sass", "less", "js", "mjs", "cjs",
		"tsx", "jsx", "vue", "svelte", "qml", "php", "asp", "aspx", "jsp",
		"shader", "glsl", "hlsl", "vert", "frag", "geom", "comp", "tesc", "tese",
		"wgsl",
		// Shell / 脚本
		"sh", "bash", "zsh", "fish", "ksh", "bat", "cmd", "ps1", "psm1", "awk", "sed",
		// 编程语言
		"py", "pyw", "c", "h", "cpp", "cxx", "cc", "hpp", "hxx", "java", "kt", "kts",
		"go", "rs", "rb", "pl", "lua", "sql", "r", "swift", "dart", "proto", "scala",
		"clj", "cljs", "ex", "exs", "erl", "hs", "ml", "mli", "nim", "zig", "v", "pas",
		"pp", "d", "groovy", "gradle", "f", "f90", "f95", "asm", "s", "nasm", "cob",
		"jl", "tcl", "vb", "fs", "fsi", "fsx", "vala", "coffee", "elm",
		"tex", "bib", "sty", "rake", "gemspec",
		// 构建 / VCS
		"cmake", "tf", "tfvars", "hcl", "diff", "patch", "properties", "manifest", "env",
		"mk", "mak", "ninja", "bazel", "bzl", "feature",
		"desktop", "service", "reg", "nsi", "nsh", "sbt",
		"wxs", "wxl", "wxi", "wixproj", "vcxproj", "vbproj", "fsproj",
		"tsconfig", "jsconfig", "xaml", "qrc", "qss", "glade",
		"m3u", "m3u8", "cue", "url",
		// 其他文本
		"log", "rtf", "pem", "pfa", "bdf", "ovf",
	};
	return exts;
}

const HashSet<String> &FileUtils::_binary_extensions() {
	static const HashSet<String> exts = {
		// 图像
		"png", "jpg", "jpeg", "jpe", "jfif", "gif", "webp", "bmp", "tif", "tiff", "tga",
		"ico", "cur", "psd", "exr", "ktx", "ktx2", "basis", "dds", "pvr", "astc",
		"ctex", "stex", "pkm", "etc2", "heic", "heif", "avif", "jxl", "pcx", "xcf",
		"svgz", "cr2", "nef", "arw", "dng", "raf", "mrw", "pef", "rw2", "orf",
		"jp2", "j2k",
		// 字体
		"ttf", "otf", "woff", "woff2", "eot", "fon", "pcf", "ttc", "pfb", "dfont",
		// 音视频
		"mp3", "mp4", "m4a", "m4v", "mov", "avi", "wav", "ogg", "oga", "ogv", "flac",
		"aac", "wma", "webm", "mkv", "flv", "swf", "mid", "midi", "aif", "aiff", "au",
		"caf", "mka", "ape", "opus", "wv", "tak",
		"wmv", "asf", "rmvb", "vob", "m2ts", "mts", "3gp", "3g2", "f4v", "amr",
		// 压缩 / 归档
		"zip", "gz", "gzip", "bz2", "bzip2", "7z", "rar", "tar", "tgz", "xz", "zst",
		"lz4", "cab", "z", "lha", "lzh", "arj", "war", "ear", "jar", "apk", "aab",
		"deb", "rpm", "dmg", "pkg", "msi", "xar", "alz", "egg", "ipa", "pak",
		"lzma", "lz",
		// 文档
		"pdf", "doc", "docx", "xls", "xlsx", "ppt", "pptx", "odt", "ods", "odp", "epub",
		"mobi", "azw", "azw3", "cbz", "cbr", "djvu",
		"docm", "xlsm", "pptm", "dotx", "xltx", "potx", "xps", "oxps",
		// 可执行 / 库
		"dll", "so", "dylib", "exe", "bin", "pyc", "class", "lib", "a", "o",
		"com", "elf", "macho", "wixobj",
		"wasm", "pyd", "node",
		// 扩展包
		"vsix", "xpi", "crx", "oxt",
		// Godot 二进制资源
		"res", "scn", "pck",
		// 3D 模型（二进制）
		"glb", "3ds", "max",
		// 数据库
		"db", "sqlite", "sqlite3", "mdb", "accdb", "dbf", "sqlitedb",
		// 磁盘 / 镜像 / 其他
		"iso", "img", "vmdk", "vdi", "vhd", "vhdx", "ova", "wim", "blob",
		"wmf", "emf", "emz", "wmz", "p12", "pfx", "der", "jks",
	};
	return exts;
}

const HashSet<String> &FileUtils::_text_basenames() {
	static const HashSet<String> names = {
		".gitignore", ".gitattributes", ".gitmodules", ".editorconfig", ".dockerignore",
		".npmignore", ".npmrc", ".yarnrc", ".env", ".env.local", ".env.example", ".envrc",
		".babelrc", ".prettierrc", ".eslintrc", ".hgignore", ".hgrc", ".cvsignore",
		".clang-format", ".clang-tidy", ".git-blame-ignore-revs", ".gitconfig",
		".ruby-version", ".python-version", ".node-version", ".rust-version",
		".tool-versions", ".htaccess", ".rspec", ".pryrc", ".nvmrc", ".rvmrc", ".bowerrc",
		"makefile", "gnumakefile", "dockerfile", "vagrantfile", "gemfile",
		"rakefile", "capfile", "guardfile", "podfile", "brewfile",
		"readme", "license", "copying", "notice", "changelog",
	};
	return names;
}

const HashSet<String> &FileUtils::_binary_basenames() {
	static const HashSet<String> names = {
		".ds_store", "thumbs.db",
	};
	return names;
}

bool FileUtils::is_text_file(const String &p_file, int p_max_read_bytes) {
	const String extension = p_file.get_extension().to_lower();
	if (_text_extensions().has(extension)) {
		return true;
	}
	if (_binary_extensions().has(extension)) {
		return false;
	}

	const String base_name = p_file.get_file().to_lower();
	if (_text_basenames().has(base_name)) {
		return true;
	}
	if (_binary_basenames().has(base_name)) {
		return false;
	}

	if (!FileAccess::exists(p_file)) {
		return false;
	}

	// 扩展名不可信时，用文件头部是否存在 NUL 字节判断二进制。
	const Ref<FileAccess> file = FileAccess::open(p_file, FileAccess::READ);
	if (file.is_null()) {
		return false;
	}
	const PackedByteArray head_bytes = file->get_buffer(MIN(file->get_length(), p_max_read_bytes));
	file->close();

	return head_bytes.find(0) == -1;
}