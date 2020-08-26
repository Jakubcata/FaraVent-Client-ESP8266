echo uploading...
ls -la .pio/build/wemosbat_development/firmware.bin
curl -H  "accept: application/json" -F 'binary=@.pio/build/wemosbat_development/firmware.bin' 'http://admin.faravent.jakubcata.eu/api/uploadBinary?version=0.01&branch=master&board=wemosbat'
echo uploaded
