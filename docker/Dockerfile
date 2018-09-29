FROM debian:stretch

RUN apt-get update && \
	apt-get install -y \
		python \
		gcc \
		wget

RUN wget http://prdownloads.sourceforge.net/scons/scons-3.0.0.tar.gz

RUN tar -xvzf scons-3.0.0.tar.gz

RUN python ./scons-3.0.0/setup.py install