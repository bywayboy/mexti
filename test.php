<?php

if(extension_loaded('mexti')){
    echo "mexti.so is loaded!\n";
}else{
    echo "mexti.so is not found!\n";
    return;
}

class MyNode extends \mexti\MinHeapNode {
    protected int $key;

    public function __construct(int $id)
    {
        $this->key = $id;   
    }

    public function getKey(): int{ return $this->key;}
    /**
     * 实现一个将自身和另外一个 node比较的的方法
     */
    public function compare(\mexti\MinHeapNode $b) : int
    {
        if($this->key > $b->key) return 1;
        return 0;
    }

    /**
     * 当 key 发生改变的时候, 自动调整自身在 MinHeap 中的顺序.
     */
    public function __set($name, $value)
    {
        switch($name){
        case 'key':
            $this->key = $value;
            $this->adjust();
        }
    }
    public function hello(){
        echo "MyNode::hello {$this->key} \n";
    }
}


$heap = new \mexti\MinHeap();

function addnodes($heap, int $count){
    echo "add {$count} nodes...\n";
    for($i=0;$i < $count; $i++){
        $heap->insert(new MyNode(rand(0,999)));
    }
}

//addnodes($heap, 3);
//$node = new MyNode(1000);

$heap->insert(new MyNode(0));
$heap->insert(new MyNode(1));
$heap->insert(new MyNode(2));


while(!$heap->isEmpty())
{
    $node = $heap->top();
    if($node->getKey() > 0 ){
        $heap->extract()->hello();
        continue;
    }else{
        if($node->adjust()){
            echo "无需再次遍历...\n";
            break;
        }
        continue;
    }
    break;
}


while(!$heap->isEmpty())
{
    $heap->extract()->hello();
}

$lua  = new \mexti\LuaVM('
print("Hello Lua")

function dump(table)
    for key, val in pairs(table) do
        if type(val) == "table" then
            print("xstart", key, dump(val))
        else
            print("xstart", key, val)
        end
    end
end

function test(xstart, to, pledge, price)
    dump(xstart)
    print(xstart ,to, pledge, price)

    return xstart , {1,2,3}
end
');

$arg1 = new \stdClass();
$arg1->username = 'bywayboy';
$arg1->sex = 2;
$arg1->sub = new \stdClass();
$arg1->sub->name='cjy';
$arg1->sub->sex=1;
$b = 'bad value';

list($a,$c) = $lua->call("test", $arg1,2,3,4);
echo json_encode([$a, $c]) . "\n";
echo "mexti\\MinHeap::count() = " . $heap->count()."\n";


echo "\mexti\\Crypto::sm3(s, false) =" . \mexti\Crypto::sm3("bywayboy") ."\n";
echo "\mexti\\Crypto::sm3(s, true)  =" . \mexti\Crypto::sm3("bywayboy", true) ."\n";

echo "\n=================== 开始测试人脸识别 ====================\n";

try{
    \mexti\Face::init();
}catch(\mexti\FaceException $e){
    echo $e->getMessage(). $e->getCode(). "\n";
}

try{
    $face = new \mexti\Face(file_get_contents('12.jpg'), 100);
    $bestIdx = $face->best();
    $quality = $face->quality($bestIdx);
    $feature = base64_encode($face->feature($bestIdx));
    echo "人脸数: {$face->num}, 尺寸:{$face->width}, {$face->height}, 质量最好的头像是: {$bestIdx}, 质量评分:{$quality}\n";
    echo "人脸特征: {$feature}\n";
}catch(\Throwable $e){
    echo $e->getMessage(). $e->getCode(). "\n";
}

echo "测试解码照片(无参数): " . \mexti\Face::decode() . "\n";
echo "测试解码照片(null): " . \mexti\Face::decode(null) . "\n";
echo "测试解码照片(无加密): " . \mexti\Face::decode(123445) . "\n";

echo "\n==================== 开始测试指纹 ====================\n";
$smallFinger = \mexti\Finger::compress('QwEREgELVwAAAAAAAAAAAAAAACUBmv///////////3cLpvxQGrT8ohqp/GYkuPxWNGf+YT60/B9EwvxgRAD8k1ap/OhboPxwb7f8zHit/Dd+yPyYm7f8UKgO/Feyv/zQvbD8w79h/lHEEvy/xa78Us3N/H/3yfy1BLT9ghLZ/Z0j0P1HJjX9tSTN/Q4o7/32Ko79gjEm/acwGf2ROO/95DeY/bY5ZP/FQ0n/slEz/91PlP0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADLAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAKY=');
echo "压缩后指纹长度:".strlen($smallFinger)."字节.\n";

