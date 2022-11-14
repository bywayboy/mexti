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
        echo "MyNode::__dstruct()\n";
    }
    /**
     * 实现一个将自身和另外一个 node比较的的方法
     */
    public function compare(\mexti\MinHeapNode $b):int
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
}


$a = new MyNode(1);
$b = new MyNode(1);

$b->key = 100;

print("a->compare(b) = " . $a->compare($b)) . "\n";

$heap = new \mexti\MinHeap();


echo "mexit\\MinHeap::count() = " . $heap->count()."\n";