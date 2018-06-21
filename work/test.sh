#!/bin/bash
config_count=4
mongos_count=2
shard_count=2
replica_count=4

cm cluster nodes > node.list
cat node.list
return
cut -d ' ' -f 2 node.list > ip.list
cat ip.list > mongo_all.txt
echo '[mongodb_all]' > inventory.txt
cat mongo_all.txt >> inventory.txt

head -$config_count ip.list > config.list
tail -n +$(expr $config_count + 1) ip.list > ip.list.1
head -$config_count mongo_all.txt > config.txt
tail -n +$(expr $config_count + 1) mongo_all.txt > mongo_work.1
echo '[mongodb_config]' >> inventory.txt
sed 's/$/ REPSET_NM=repset_config/' config.txt > config1.txt
cat config1.txt >> inventory.txt
echo '[mongodb_config_1]' >> inventory.txt
head -1 config.txt >> inventory.txt

head -$config_count ip.list.1 > mongos.list
tail -n +$(expr $config_count + 1) ip.list.1 > ip_shard.list.1
head -$mongos_count mongo_work.1 > mongos.txt
tail -n +$(expr $mongos_count + 1) mongo_work.1 > mongo_shard_work.1
echo '[mongodb_mongos]' >> inventory.txt
one_config_ip_address=$(head -n 1 config.list)
sed "s/$/ CONFIG_REPSET=repset_config CONFIG_HOSTNAME=$one_config_ip_address/" mongos.txt > mongos1.txt
cat mongos1.txt >> inventory.txt
echo '[mongodb_mongos_1]' >> inventory.txt
head -1 mongos.txt >> inventory.txt
head -1 mongos.txt > mongos_main_ip.txt

current_shard=1
echo "[mongodb_shard]" >> inventory.txt 
while [ $current_shard -le $shard_count ]
do
	head -$replica_count ip_shard.list.$current_shard > shard_$current_shard.list
	tail -n +$(expr $replica_count + 1) ip_shard.list.$current_shard > ip_shard.list.$(expr $current_shard + 1)
	head -$replica_count mongo_shard_work.$current_shard > shard_$current_shard.txt
	tail -n +$(expr $replica_count + 1) mongo_shard_work.$current_shard > mongo_shard_work.$(expr $current_shard + 1)
#####        echo "[mongodb_shard$current_shard]" >> inventory.txt
	sed "s/$/ REPSET_NM=repset_shard$current_shard/" shard_$current_shard.txt > shard_$current_shard.1.txt
	cat shard_$current_shard.1.txt >> inventory.txt

cut -d ' ' -f 1 shard_$current_shard.txt > initiate_ips_shard${current_shard}.txt

	current_shard=$(expr $current_shard + 1)
done


#build the shard initiate file
rm -f initiate_shard*.js

shard_number=$shard_count

echo '[mongodb_shard_1]' >> inventory.txt
echo "db.getSiblingDB(\"admin\").auth(\"cluster_admin_user\", \"cluster_admin_password\" )" > add_shard$shard_number.js

while [[ $shard_number -gt 0 ]]
do
        echo "rs.initiate(
          {
            _id: \"repset_shard${shard_number}\",
            members: [" > initiate_shard$shard_number.js1

        counter=0
        while read ip_address
        do
		if [[ counter -eq 0 ]]
		then
			echo "$ip_address INITIATE_FILE=initiate_shard$shard_number.js" >> inventory.txt
			echo "sh.addShard(\"repset_shard$shard_number/$ip_address:27018\")" >> add_shard$shard_number.js
			cat add_shard$shard_number.js >> add_shards.js
		fi

                echo "      { _id : $counter, host : \"$ip_address:27018\" }," > initiate_shard$shard_number.js1
                ((counter++))
        done < initiate_ips_shard$shard_number.txt #>> initiate_shard$shard_number.js1

        sed '$ s/.$//' initiate_shard$shard_number.js1 > initiate_shard$shard_number.js

        echo '    ]
          }
        )' >> initiate_shard$shard_number.js


        ((shard_number--))
done


echo '[mongodb_shard_1]' >> inventory.txt

echo "[all:vars]" >> inventory.txt
echo "ansible_ssh_user=$user_name" >> inventory.txt
echo "MONGO_DIR=$mongodir" >> inventory.txt
