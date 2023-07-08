package com.image.library.opencv.camera

import androidx.annotation.RequiresApi
import androidx.lifecycle.LiveData
import androidx.lifecycle.MediatorLiveData

@RequiresApi(21)
internal class ForwardingLiveData<T> : MediatorLiveData<T>() {
    private var mLiveDataSource: LiveData<T>? = null
    fun setSource(liveDataSource: LiveData<T>) {
        if (mLiveDataSource != null) {
            super.removeSource(mLiveDataSource!!)
        }
        mLiveDataSource = liveDataSource
        super.addSource(liveDataSource) { value: T ->
            this.setValue(
                value
            )
        }
    }

    override fun getValue(): T? {
        return if (mLiveDataSource == null) null else mLiveDataSource!!.value
    }
}
