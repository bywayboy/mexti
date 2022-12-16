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

$lua  = new \mexti\Lua('
print("Hello Lua")
function test(xstart, to, pledge, price)
    for key, val in pairs(xstart) do
        print("xstart", key, val)
    end
    print(xstart ,to, pledge, price)
    return xstart , {1,2,3}
end
');

$arg1 = new \stdClass();

$arg1->username = 'bywayboy';
$arg1->sex = 2;
$b = 'bad value';
$c = $lua->call("test", $arg1,2,3,4);
echo json_encode($c) . "\n";
echo "mexti\\MinHeap::count() = " . $heap->count()."\n";