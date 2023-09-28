## PHP 最小堆扩展


## 如何编译

1. 修改 make.sh PHP_BASE_PATH=/usr/local/php 为你的php所在安装目录.
2. 执行 make.sh


## 关于人脸算法库

人脸特征提取、活体检测、1v1 比对 1vN查找. 采用的是 中正公司的SDK. 
如需获取授权文件请联系微信，QQ：150560739，电话15868108251
修改 `zzface/tools/MXParaSet.ini`  后执行 ./auth 获取授权信息
```ini
[MXParaSet]
IP=183.129.171.153
Port=1902
UserId=testuser
PWD=testuser
```

### mexti\MinHeap

```php
namespace mexti;

class MinHeap{
    
    /**
     * 获取内部成员数目
     */
    public function count() : int;

    /**
     * 是否为空 等价于 count() == 0
     */
    public function isEmpty() : bool;

    /**
     * 插入一个成员到最小堆中
     */
    public function insert(MinHeapNode $n) : bool|int;

    /**
     * 从最小堆中移除一个成员
     */
    public function erase(MinHeapNode $n) : bool|int;

    /**
     * 成员键值更新后, 调整在最小堆中的位置.
     * 返回值:
     *      true:  调整后 $n 位于堆头部
     *      false: 调整后 $n 不位于堆头部
     */
    public function adjust(MinHeapNode $n) : bool|int;

    /**
     * 从最小堆中弹出一个成员
     */
    public function extract() : MinHeapNode;

    /**
     * 获取最小堆中下一个将要弹出的成员(并不会弹出),
     */
    public function top() : MinHeapNode;
}
```
## \mexti\MinHeapNode

```php
namespace mexti;

class MinHeapNode{
    /**
     * 需要被继承类实现的比较方法
    */
    public abstract compare(\mexti\MinHeapNode $b) : int;

    /**
     * 是否在 MinHeap池中.
    */
    public function inHeap() : bool;

    /*
     * 获取自身所在的MinHeap
    */
    public function getHeap() : ?\mexti\MinHeap;
    /**
     * 索引更新后调整位置.
     *      true:  调整后 $n 位于堆头部
     *      false: 调整后 $n 不位于堆头部
     */
    public function adjust(): bool;
    /**
     * 从MinHeap中移除自身
    */
    public function erase() : bool;
}

class SampleNode extends MinHeapNode {
    // 比较键值
    protected int $key;
    /*
        实现比较方法
    */
    public function compare(\MinHeapNode $b) : int {
        if($this->key > $b->key) return 1;
        elseif($this->key < $b->key) return -1;
        return 0;
    }
} 
```

参考资料:
https://blog.csdn.net/manwea/article/details/103557214

https://y2k38.github.io/posts/php-extension-writing-classes/

深入理解PHP7内核之OBJECT
https://www.laruence.com/2020/03/23/5605.html

https://yaoguais.github.io/article/php/extension-php5to7.html

https://github.com/GoAnimate-Stuff-by-Zoccorus/wrapper-offline/blob/master/utilities/sourcecode/php-src-master/ext/spl/spl_heap_arginfo.h
