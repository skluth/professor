# 
FROM andrewosh/binder-base
MAINTAINER Holger Schulz <iamholger@gmail.com>
USER root
RUN apt-get update
RUN apt-get install libeigen3-dev -y
RUN pip install iminuit
#RUN wget https://users.hepforge.org/~holsch/Temp/professor-bootstrap-docker-2.2.0 -O professor-bootstrap-docker && chmod a+x professor-bootstrap-docker 
#RUN ./professor-bootstrap-docker

RUN wget http://www.hepforge.org/archive/professor/Professor-2.2.0.tar.gz && tar xzf Professor-2.2.0.tar.gz && cd Professor-2.2.0 && CXXFLAGS="-O4" make all && PREFIX=/ make install &&  cd /
RUN rm -rf Professor-2.2.0.tar.gz Professor-2.2.0
RUN wget http://www.hepforge.org/archive/yoda/YODA-1.6.6.tar.gz
RUN tar xzf YODA-1.6.6.tar.gz
RUN cd YODA-1.6.6 && ./configure --prefix=/ && make install
ENV PYTHONPATH /lib/python2.7/site-packages
RUN rm -rf /home/main/{build,YODA-1.6.6,YODA-1.6.6.tar.gz,professor-bootstrap-docker}
RUN pip install fileupload
RUN jupyter nbextension install --py fileupload
RUN jupyter nbextension enable --py fileupload
RUN jupyter nbextension enable --py --sys-prefix widgetsnbextension
