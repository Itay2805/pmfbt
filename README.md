# Poor Man's Full Body Tracking

The aim of this project is to provide full-body tracking capability to vr games using only a phone/webcam.

## Build

Simply use make:
```sh
make PLATFORM={windows,linux} -j
```

this will output for you a driver under `bin/RELEASE/{windows,linux}_amd64_pmfbt_driver.{dll,so}`
