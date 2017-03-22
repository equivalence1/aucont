FROM eabatalov/aucont-test-base
USER 0
RUN apt-get install -y cgroup-tools &&\
    apt-get install -y iptables
USER 1000
WORKDIR /test/scripts
CMD ./test.py
