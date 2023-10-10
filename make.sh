#!/bin/bash


if [ ! -f zzface.tar.gz ]; then
    wget -O zzface.tar.gz  https://exti.cc/depends/zzface.tar.gz
fi

if [ ! -d zzface ]; then
    tar -xvf zzface.tar.gz
    cp zzface/* /lib/x86_64-linux-gnu/
    pushd  /lib/x86_64-linux-gnu
    ln -s MXFaceApiDebian7.so libMXFaceApiDebian7.so
    popd
fi

if [ ! -f zzfinger.tar.gz ]; then
    wget -O zzface.tar.gz  https://exti.cc/depends/zzfinger.tar.gz
fi

if [ ! -d zzfinger ]; then
    tar -xvf zzfinger.tar.gz
    cp zzfinger/* /lib/x86_64-linux-gnu/
    pushd  /lib/x86_64-linux-gnu
    popd
fi



PHP_BASE_PATH=/usr/local/php

if [ ! -f Makefile ]; then
    ${PHP_BASE_PATH}/bin/phpize
    ./configure --with-php-config=${PHP_BASE_PATH}/bin/php-config
fi
make && make install

PHP_CONFIG_FILE=mexti.ini
cp ${PHP_BASE_PATH}/etc/php.ini  ${PHP_CONFIG_FILE}

function CheckPHPConfig() {

    PHP_EXTENSIONS=("mexti.so" "gd.so")

    echo -e "\033[32m==================== 开始检PHP查配置 ====================\033[0m"
    echo -e "\033[34m[NOTICE] PHP配置文件: ${PHP_CONFIG_FILE} \033[0m"
    for EXTENSION_NAME in ${PHP_EXTENSIONS[@]}
    do
        TEMP_VAR=$(sed "/^extension=${EXTENSION_NAME}/!d; s/.*=//" ${PHP_CONFIG_FILE} 2>/dev/null)
        if [ "$TEMP_VAR" = "" ]; then
            echo -e "\033[34m[NOTICE] 未找到 $EXTENSION_NAME 扩展加载指令 自动加入 \033[0m"
            sed -i "/;extension=xsl/i\extension=${EXTENSION_NAME}" ${PHP_CONFIG_FILE}
            TEMP_VAR=$(sed "/^extension=${EXTENSION_NAME}/!d; s/.*=//" ${PHP_CONFIG_FILE} 2>/dev/null)
            if [ "$TEMP_VAR" = "" ]; then
                echo -e "\033[31m[ERROR] 加入扩展 $EXTENSION_NAME : $TEMP_VAR1 加载指令 失败! \033[0m"
            else
                echo -e "\033[32m[PASS] 添加扩展加载指令 $EXTENSION_NAME 完成! \033[0m"
            fi
        else
            echo -e "\033[35m[PASS] 检查扩展 $EXTENSION_NAME \033[0m"
        fi
    done
    echo -e "\033[32m==================== PHP配置检查完成 ====================\033[0m"
}

CheckPHPConfig;


cat  <<EOF 	>> ${PHP_CONFIG_FILE}

[mexti]
; 是否开启人脸检测模块
mexti.face_on = Off;
; 人脸检测授权模型文件位置
mexti.face_license = "/usr/local/php/etc";
; 1:N 最大搜索数目
mexti.face_serch_num = 5000;

EOF


$PHP_BASE_PATH/bin/php -c mexti.ini -f test.php
