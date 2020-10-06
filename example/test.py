# from conda.core.portability import binary_replace
import sys, re, json, os
import stat

# from conda
def binary_replace(data, a, b):
    """
    Perform a binary replacement of `data`, where the placeholder `a` is
    replaced with `b` and the remaining string is padded with null characters.
    All input arguments are expected to be bytes objects.
    """
    def replace(match):
        occurances = match.group().count(a)
        padding = (len(a) - len(b)) * occurances
        if padding < 0:
            raise _PaddingError
        return match.group().replace(a, b) + b'\0' * padding

    original_data_len = len(data)
    pat = re.compile(re.escape(a) + b'([^\0]*?)\0')
    data = pat.sub(replace, data)
    assert len(data) == original_data_len

    return data

with open(sys.argv[1], 'rb+') as fi:
	data = fi.read()

with open(sys.argv[2]) as json_fi:
	json_files = json.loads(json_fi.read())

root_prefix = os.environ["CONDA_PREFIX"] if int(os.environ["CONDA_SHLVL"]) == 1 else os.environ["CONDA_PREFIX_1"]
pkgs_cache = os.path.join(root_prefix, 'pkgs')

for f in json_files:
	with open(os.path.join(pkgs_cache, f['dist_name'], 'info/repodata_record.json')) as repodata_json:
		repodata = json.load(repodata_json)
		f.update(repodata)

json_bytes = json.dumps(json_files).encode('utf-8')
self_size = len(data)
json_size = len(json_bytes)
payload_size = 0

outfile = sys.argv[1] + '.done'
with open(outfile, 'wb') as fo:

	for repodata in json_files:
		full_file = os.path.join(pkgs_cache, repodata['fn'])
		payload_size += os.path.getsize(full_file)

	print(f"Sizes: {self_size}, {json_size}, {payload_size}")
	sizes_str = f"{self_size};{json_size};{payload_size}"
	data = binary_replace(data, b'thisisaverylongstringthatshouldbereplacedbythebuildprocessabcdef', sizes_str.encode('utf-8'))

	fo.write(data)
	fo.write(json_bytes)

	for repodata in json_files:
		print(repodata['dist_name'])
		full_file = os.path.join(pkgs_cache, repodata['fn'])
		with open(full_file, 'rb') as pkg_in:
			fo.write(pkg_in.read())

st = os.stat(outfile)
os.chmod(outfile, st.st_mode | stat.S_IEXEC)
