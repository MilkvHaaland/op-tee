
demos := acipher cipher curve25519 ecdh ecdsa dsa efuse hash hello_world secure_storage trng 
.PHONY: all $(demos) clean
all: $(demos)

acipher: 
	cd $@/host && make
	cd $@/ta && make
	
cipher: 
	cd $@/host && make
	cd $@/ta && make
	
curve25519: 
	cd $@/host && make
	cd $@/ta && make

ecdh: 
	cd $@/host && make
	cd $@/ta && make

ecdsa: 
	cd $@/host && make
	cd $@/ta && make

efuse: 
	cd $@/host && make

hash: 
	cd $@/host && make
	cd $@/ta && make

hello_world: 
	cd $@/host && make
	cd $@/ta && make

secure_storage: 
	cd $@/host && make
	cd $@/ta && make

trng: 
	cd $@/host && make
	cd $@/ta && make
dsa: 
	cd $@/host && make
	cd $@/ta && make
	
clean:
	for i in $(demos) ; do \
	  if [ "$$i" = "efuse" ]; then \
	    cd $$i/host && make clean && cd ../../; \
	  else \
	    cd $$i/host && make clean && cd ../ta && make clean && cd ../../; \
	  fi; \
	done
	
