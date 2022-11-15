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

    public function __destruct()
    {
        echo "MyNode::__dstruct({$this->key})\n";
    }
    /**
     * 实现一个将自身和另外一个 node比较的的方法
     */
    public function compare(\mexti\MinHeapNode $b) : int
    {
        if($this->key === $b->key) return 0;
        elseif($this->key < $b->key) return 1;
        else return -1;
        
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
        $heap->insert( new MyNode(rand(0,999)));
    }
}

addnodes($heap, 2);

$node = $heap->extract();
$node->hello();
echo "mexit\\MinHeap::count() = " . $heap->count()."\n";