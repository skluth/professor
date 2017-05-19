# 
FROM fedora:25
MAINTAINER Holger Schulz <iamholger@gmail.com>
ENV LD_LIBRARY_PATH /usr/lib
ENV PYTHONPATH /usr/lib64/python2.7/site-packages
RUN dnf install make eigen3-devel gcc-c++ redhat-rpm-config python-devel wget tar python-matplotlib  root-python.x86_64 less -y && dnf clean all
RUN pip install --upgrade pip
RUN pip install iminuit
RUN pip install cython==0.25.1
ENV PATH /usr/local/sbin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/contrib
RUN wget http://www.hepforge.org/archive/professor/Professor-2.2.2beta2.tar.gz
RUN tar xzf Professor-2.2.2beta2.tar.gz && cd Professor-2.2.2beta2 && PREFIX=/ make install
RUN wget http://www.hepforge.org/archive/yoda/YODA-1.6.6.tar.gz
RUN tar xzf YODA-1.6.6.tar.gz
RUN cd YODA-1.6.6 && ./configure --prefix=/ --enable-root && make install
RUN rm -rf YODA*
RUN pip install fileupload
RUN jupyter nbextension install --py fileupload
RUN jupyter nbextension enable --py fileupload
RUN jupyter nbextension enable --py --sys-prefix widgetsnbextension
