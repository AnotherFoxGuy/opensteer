@echo off 
REM Script to copy the required files into the relevant folders

REM Setup path variables

set ogreRelease=C:\OGRE_ENGINE\ogrenew\Samples\Common\bin\Release
set ogreDebug=C:\OGRE_ENGINE\ogrenew\Samples\Common\bin\Debug
set editorRelease=C:\OGRE_ENGINE\OgreSteer\bin\release
set editorDebug=C:\OGRE_ENGINE\OgreSteer\bin\debug
set editorDataFiles=C:\OGRE_ENGINE\OgreSteer\data\CEGUI_datafiles
set ceguiBin=C:\OGRE_ENGINE\cegui_mk2\bin
set ceguiXMLRef=C:\OGRE_ENGINE\cegui_mk2\XMLRefSchema

ECHO ON

@echo -----------------
@echo Copying OGRE Dlls
@echo -----------------

COPY %ogreRelease%\OgreMain.dll %editorRelease% 
COPY %ogreRelease%\OgrePlatform.dll %editorRelease% 
COPY %ogreRelease%\Plugin_ParticleFX.dll %editorRelease% 
COPY %ogreRelease%\Plugin_OctreeSceneManager.dll %editorRelease% 
COPY %ogreRelease%\Plugin_CgProgramManager.dll %editorRelease% 
COPY %ogreRelease%\RenderSystem_Direct3D9.dll %editorRelease% 
COPY %ogreRelease%\RenderSystem_GL.dll %editorRelease% 
COPY %ogreRelease%\OgreGUIRenderer.dll %editorRelease% 

COPY %ogreDebug%\OgreMain_d.dll %editorDebug% 
COPY %ogreDebug%\OgrePlatform_d.dll %editorDebug% 
COPY %ogreDebug%\Plugin_ParticleFX.dll %editorDebug% 
COPY %ogreDebug%\Plugin_OctreeSceneManager.dll %editorDebug% 
COPY %ogreDebug%\Plugin_CgProgramManager.dll %editorDebug% 
COPY %ogreDebug%\RenderSystem_Direct3D9.dll %editorDebug% 
COPY %ogreDebug%\RenderSystem_GL.dll %editorDebug% 
COPY %ogreDebug%\OgreGUIRenderer_d.dll %editorDebug% 

@echo ------------------
@echo Copying CEGUI Dlls
@echo ------------------

COPY %ceguiBin%\CEGUIBase.dll %editorRelease% 
COPY %ceguiBin%\CEGUIFalagardBase.dll %editorRelease% 

COPY %ceguiBin%\CEGUIBase_d.dll %editorDebug% 
COPY %ceguiBin%\CEGUIFalagardBase_d.dll %editorDebug% 

@echo ----------
@echo CEGUI XMDs
@echo ----------

COPY %ceguiXMLRef%\Font.xsd %editorDataFiles%\fonts 
COPY %ceguiXMLRef%\Imageset.xsd %editorDataFiles%\imagesets 
COPY %ceguiXMLRef%\GUIScheme.xsd %editorDataFiles%\schemes 
COPY %ceguiXMLRef%\Falagard.xsd %editorDataFiles%\looknfeel 

PAUSE