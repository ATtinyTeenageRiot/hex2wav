#!/bin/bash
REMOTE="xcorex@172.16.225.134"
PORT="22"

cwd=$(pwd)

REPO_URL="https://github.com/ATtinyTeenageRiot/hex2wav.git"

REPO_DIR="~/hex2wav"

STAGING_DIR="./tools/staging"

DESTINATION_REPO="."

COMMIT_HASH="b35be2452cdd206067c3aad125b7656fb84813e7"

#ssh ${REMOTE} -p ${PORT} bash -c "'
#cd "${REPO_DIR}/avrdude-6.0rc1"
#./avrdude.exe -cusbasp -pt85 -C ./avrdude.conf
#'"

ssh ${REMOTE} -p ${PORT} bash -c "'
. /opt/windows_32/bin/win-builds-switch 32                                                                                                 
git clone ${REPO_URL}
pwd
cd ${REPO_DIR}
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

cp hex2wav.exe ../release

git log -1 > ../release/changelog.txt

cd "${REPO_DIR}/release"
pwd
ls "${REPO_DIR}/release"

'"

mkdir -p "${cwd}/${DESTINATION_REPO}/windows"
scp -P ${PORT} "${REMOTE}:${REPO_DIR}/release/*" "${cwd}/${DESTINATION_REPO}/windows"