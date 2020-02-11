FROM centos:8

# Install dependencies
RUN yum install --assumeyes dnf-plugins-core && \
	yum config-manager --set-enabled PowerTools && \
	yum update --assumeyes --skip-broken && \
	yum install --assumeyes https://repo.mysql.com//mysql80-community-release-el8-1.noarch.rpm && \
	yum install --assumeyes clang mysql mysql-connector-c++ mysql-connector-c++-devel cmake3 ninja-build openssl openssl-devel

# Copy Quesync source
COPY . /app

# Change work dir
WORKDIR /app/server

# Build server
RUN export CC=clang CXX=clang++ && ./build.sh

CMD [ "sh", "-c", "build/bin/server -h ${QUESYNC_DB_HOST}" ]