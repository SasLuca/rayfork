/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.android.gles3jni

import android.app.Activity
import android.content.Context
import android.content.pm.ActivityInfo
import android.content.res.AssetManager
import android.opengl.GLES30
import android.opengl.GLES32
import android.opengl.GLSurfaceView
import android.os.Bundle
import android.util.DisplayMetrics
import android.util.Log
import android.view.MotionEvent
import android.view.View
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

const val PressedInputEvent = 1
const val RaisedInputEvent  = 2
const val MovedInputEvent   = 3

object GLES3JNILib
{
    external fun init(assetManager: AssetManager, density: Float, width: Int, height: Int, internal_storage_path: String)
    external fun onResize(width: Int, height: Int)
    external fun processPointerInput(event: Int, pointerIndex: Int, x: Int, y: Int)
    external fun step()
}

//https://stackoverflow.com/questions/39582808/android-motionevent-pointer-index-confusion
fun getAdjustmentForPointerIndex(event: MotionEvent, pointerIndex: Int): Int
{
    var adjust = 0

    for (i in 0 until event.pointerCount)
    {
        // Get Actual Pointer Index of touch
        val adjustedPointerIndex = event.getPointerId(i)

        // If we've found the current touch's pointer index AND
        // the pointer index doesn't equal the sequential event's
        // pointers
        if (adjustedPointerIndex == pointerIndex && i != adjustedPointerIndex)
        {
            adjust = adjustedPointerIndex - i
            break
        }
    }

    return pointerIndex - adjust
}

class GLES3JNIView(context: Context, val assetManager: AssetManager, val density: Float, val screenWidth: Int, val screenHeight: Int, val internalStoragePath: String) : GLSurfaceView(context)
{
    class Renderer(val assetManager: AssetManager, val density: Float, val screenWidth: Int, val screenHeight: Int, val internalStoragePath: String) : GLSurfaceView.Renderer
    {
        override fun onDrawFrame(gl: GL10)
        {
            GLES3JNILib.step()
        }

        override fun onSurfaceChanged(gl: GL10, width: Int, height: Int)
        {

        }

        override fun onSurfaceCreated(gl: GL10, config: EGLConfig)
        {
            GLES3JNILib.init(assetManager, density, screenWidth, screenHeight, internalStoragePath)
        }
    }

    init
    {
        // Pick an EGLConfig with RGB8 color, 16-bit depth, no stencil
        setEGLConfigChooser(8, 8, 8, 0, 16, 0)
        setEGLContextClientVersion(3)
        setRenderer(Renderer(assetManager, density, screenWidth, screenHeight, internalStoragePath))
        setPreserveEGLContextOnPause(true)
    }

    override fun onTouchEvent(event: MotionEvent): Boolean
    {
        val action       = event.action
        val pointerIndex = event.getPointerId(event.actionIndex)
        val pointerId    = getAdjustmentForPointerIndex(event, pointerIndex)

        if (pointerId >= 5) return true;

        val x = event.getX(pointerId).toInt()
        val y = event.getY(pointerId).toInt()

        when (action)
        {
            MotionEvent.ACTION_DOWN, MotionEvent.ACTION_POINTER_DOWN -> {
                queueEvent { GLES3JNILib.processPointerInput(PressedInputEvent, pointerIndex, x, y) }
            }

            MotionEvent.ACTION_MOVE -> {
                queueEvent { GLES3JNILib.processPointerInput(MovedInputEvent, pointerIndex, x, y) }
            }

            MotionEvent.ACTION_UP, MotionEvent.ACTION_POINTER_UP, MotionEvent.ACTION_CANCEL -> {
                queueEvent { GLES3JNILib.processPointerInput(RaisedInputEvent, pointerIndex, x, y) }
            }
        }

        return true
    }
}

class GLES3JNIActivity : Activity()
{
    lateinit var view: GLES3JNIView
    lateinit var assetManager: AssetManager

    var density = 0f
    var screenWidth = 0
    var screenHeight = 0
    var internalStoragePath = ""

    override fun onCreate(bundle: Bundle?)
    {
        super.onCreate(bundle)

        System.loadLibrary("gles3jni")

        // Force portait
        requestedOrientation = ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE

        // Get info
        assetManager = assets

        val metrics = DisplayMetrics()
        windowManager.defaultDisplay.getRealMetrics(metrics)

        density = metrics.density * 160f
        screenWidth = metrics.widthPixels
        screenHeight = metrics.heightPixels
        internalStoragePath = filesDir.absolutePath

        // Init GL view
        view = GLES3JNIView(application, assetManager, density, screenWidth, screenHeight, internalStoragePath)
        setContentView(view)
    }

    override fun onWindowFocusChanged(hasFocus: Boolean)
    {
        super.onWindowFocusChanged(hasFocus)

        if (hasFocus)
        {
            window.decorView.systemUiVisibility = (View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                    or View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                    or View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    or View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                    or View.SYSTEM_UI_FLAG_FULLSCREEN
                    or View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY)
        }
    }

    override fun onPause()
    {
        super.onPause()
        view.onPause()
    }

    override fun onResume()
    {
        super.onResume()
        view.onResume()
    }
}