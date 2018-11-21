package com.iget.datareporter;

public interface IReport {
    /**
     * @param key 返回唯一标识符
     * @param data 返回需要上报的数据，数据是有序的
     */
    void upload(long key, String[] data);
}
