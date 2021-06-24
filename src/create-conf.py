import sys

mol  = (sys.argv)[1]
step = "0.00001"
itype = "RNA"

print("configuration maker")
print("===================")
print("input format - comma separated list of real numbers\n")

print("Temperatures :", end=' ')
temps = list(set(input().split(' ')))
temps = ["20.0"] if len(temps) == 0 else temps

print("dT (default = 0.00001) :", end=' ')
st = input()
step = st if len(st) > 0 else step

print("interaction type (default = RNA) :", end=' ')
it = input().strip()
itype = it if len(st) > 0 else itype

template_file = open("../../.config/conf_template")
file_data = "".join(template_file.readlines())
template_file.close()

for i in temps:
    conf_data = file_data
    newconf = open("input_"+i.replace(".", "_"), "w")
    newconf.write(
        conf_data.replace("_MOL_", mol)
                 .replace("_STEP_", step)
                 .replace("_TEMPD_", i+"C")
                 .replace("_TEMP_", i.replace(".", "_")+"C")
                 .replace("_ITYPE_", itype)
    )
    newconf.close()
