CFLAGS="-I../../include -O2 -Wl,-rpath,'/usr/local/lib/LabVIEW-8.2/linux'"
LDFLAGS="-lnidaqmx /usr/local/lib/LabVIEW-8.2/linux/libstdc++.so.5"

all: nav_radios debug_inputs

nav_radios: nav_radios.c
	gcc $(CFLAGS) nav_radios.c $(LDFLAGS) -o nav_radios

debug_inputs: debug_inputs.c
	gcc $(CFLAGS) debug_inputs.c $(LDFLAGS) -o debug_inputs
