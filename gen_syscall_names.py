
# generate c++ code which returns vector with syscall names

out_fname = 'syscall_names.cc'

in_data = ''

'''
file_name = '/root/kernel/linux-stable/arch/x86/entry/syscalls/syscall_64.tbl'
with open(file_name) as file:
    in_data = file.read()
    file.close()
'''

import urllib.request
file_url = 'https://raw.githubusercontent.com/torvalds/linux/master/arch/x86/entry/syscalls/syscall_64.tbl'
http_resp = urllib.request.urlopen(file_url)
if http_resp.status != 200:
  print('cannot retrieve ' + file_url)
  exit()
in_data = str(http_resp.read(), 'utf-8')



out = open(out_fname, 'w')

out.write('#include <vector>\n')
out.write('#include <string>\n')
out.write('std::vector<std::string> get_syscall_name_vector() {\n')
out.write('  std::vector<std::string> out;\n')
out.write('  out.resize(1024);\n\n')


in_lines = in_data.split('\n')

for line in in_lines:
    line = line.strip()

    if not line :
        continue
    if line.startswith("#") :
        continue

    splited = line.split()
    if len(splited) < 3:
        continue

    print(splited)
    syscall_id = splited[0]
    syscall_name = splited[2]
    out.write('  out[{0}] = "{1}";\n'.format(syscall_id, syscall_name))

out.write('  return out;\n')
out.write('}\n')

out.close()

print('output is in '+out_fname)