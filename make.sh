#!/bin/bash

PHP_BASE_PATH=/usr/local/php

${PHP_BASE_PATH}/bin/phpize
./configure --with-php-config=${PHP_BASE_PATH}/bin/php-config
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


# cat <<EOF >>  facealg.ini
# [facealg]
# facealg.license = ${PHP_BASE_PATH}/etc
# facealg.serch_num = 5000
# EOF

${PHP_BASE_PATH}/bin/php -c mexti.ini -f test.php
