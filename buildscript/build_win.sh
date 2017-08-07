#!/bin/bash
REMOTE="xcorex@172.16.225.134"
PORT="22"

cwd=$(pwd)

REPO_URL="https://github.com/ATtinyTeenageRiot/hex2wav.git"

REPO_DIR="/c/Users/BvSsh_VirtualUsers/hex2wav"

STAGING_DIR="./tools/staging"

DESTINATION_REPO="."

COMMIT_HASH="5d9bae1661961bc4f684596cb575b40f8a013def"

#ssh ${REMOTE} -p ${PORT} bash -c "'
#cd "${REPO_DIR}/avrdude-6.0rc1"
#./avrdude.exe -cusbasp -pt85 -C ./avrdude.conf
#'"

ssh ${REMOTE} -p ${PORT} bash -c "'
. /opt/windows_32/bin/win-builds-switch 32  
cd ~
pwd                                                                                               
git clone ${REPO_URL}
pwd
cd ${REPO_DIR}
rm -fr buildscript/windows
git fetch --all
git pull --all
git checkout ${COMMIT_HASH}
git clean -dfx
git log -1
#ls "${REPO_DIR}"

cd "${REPO_DIR}/src"
make clean
make

pwd

mkdir -p ../buildscript/windows/

cp hex2wav.exe ../buildscript/windows/
cp test.hex ../buildscript/windows/

git log -1 > ../buildscript/windows/changelog.txt

cd "${REPO_DIR}/buildscript/windows"

pwd

ls "${REPO_DIR}/buildscript/windows"

'"

mkdir -p "${cwd}/${DESTINATION_REPO}/windows"
scp -P ${PORT} "${REMOTE}:${REPO_DIR}/buildscript/windows/*" "${cwd}/${DESTINATION_REPO}/windows"