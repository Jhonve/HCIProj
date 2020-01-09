#include "MainOpenGLWidget.h"
#include <QmouseEvent>
#include <qcoreapplication.h>

#include <string>
#include <iostream>

#include <QFileDialog>

GLuint VBOObj, VAOObj;

bool MainOpenGLWidget::m_transparent = false;

MainOpenGLWidget::MainOpenGLWidget(QWidget *parent)
	:QOpenGLWidget(parent),
	m_xRot(0),
	m_yRot(0),
	m_zRot(0)
{
	if (m_transparent)
	{
		QSurfaceFormat fmt = format();
		fmt.setAlphaBufferSize(8);
		setFormat(fmt);
	}

	m_object_3d = new Objects3D("Data/bunny.off");

	setFocusPolicy(Qt::StrongFocus);
	m_video_cap = cv::VideoCapture(0);
}

MainOpenGLWidget::~MainOpenGLWidget()
{
	cleanup();
	m_video_cap.release();
	cv::destroyAllWindows();
}

QSize MainOpenGLWidget::minimumSizeHint() const
{
	return QSize(256, 256);
}

QSize MainOpenGLWidget::sizeHint() const
{
	return QSize(512, 512);
}

static void qNormalizeAngle(int &angle)
{
	while (angle < 0)
	{
		angle += 360 * 16;
	}
	while (angle > 360 * 16)
	{
		angle -= 360 * 16;
	}
}

void MainOpenGLWidget::setXRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != m_xRot)
	{
		m_xRot = angle;
		emit xRotationChanged(angle);
		update();
	}
}

void MainOpenGLWidget::setYRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != m_yRot)
	{
		m_yRot = angle;
		emit yRotationChanged(angle);
		update();
	}
}

void MainOpenGLWidget::setZRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != m_zRot)
	{
		m_zRot = angle;
		emit zRotationChanged(angle);
		update();
	}
}

void MainOpenGLWidget::cleanup()
{
	if (&m_shader->shader_program == nullptr)
	{
		return;
	}
	makeCurrent();
	delete &m_shader->shader_program;
	doneCurrent();
}

void MainOpenGLWidget::initializeGL()
{
	// shader part
	m_core = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();
	m_shader = new Shader("ShaderFiles/vertex_shader_source.vert", "ShaderFiles/fragment_shader_source.frag");

	m_core->glGenVertexArrays(1, &VAOObj);
	m_core->glGenBuffers(1, &VBOObj);
	m_core->glBindVertexArray(VAOObj);
	m_core->glBindBuffer(GL_ARRAY_BUFFER, VBOObj);
	m_core->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * (m_object_3d->m_num_faces * 3 * 9), m_object_3d->m_vertices_data, GL_STATIC_DRAW);
	m_core->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)0);
	m_core->glEnableVertexAttribArray(0);
	m_core->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	m_core->glEnableVertexAttribArray(1);
	m_core->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	m_core->glEnableVertexAttribArray(2);

	m_core->glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

	// set camera
	m_camera.setToIdentity();
	m_camera.translate(0, 0, -2);

	m_a_light_pos = QVector3D(0.0, 2.0, 3.0);
	m_b_light_pos = QVector3D(0.0, 2.0, -3.0);
	m_view_pos = QVector3D(0.0, 0.0, -2.0);

	m_is_reload = false;
}

void MainOpenGLWidget::resizeGL(int width, int height)
{
	m_proj.setToIdentity();
	m_proj.perspective(45.0f, GLfloat(width) / height, 0.01f, 100.0f);
}

void MainOpenGLWidget::paintGL()
{
	m_core->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_core->glEnable(GL_DEPTH_TEST);
	m_core->glEnable(GL_CULL_FACE);

	if (m_is_reload)
	{
		m_core->glGenVertexArrays(1, &VAOObj);
		m_core->glGenBuffers(1, &VBOObj);
		m_core->glBindVertexArray(VAOObj);
		m_core->glBindBuffer(GL_ARRAY_BUFFER, VBOObj);
		m_core->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * (m_object_3d->m_num_faces * 3 * 6), m_object_3d->m_vertices_data, GL_STATIC_DRAW);
		m_core->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)0);
		m_core->glEnableVertexAttribArray(0);
		m_core->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		m_core->glEnableVertexAttribArray(1);
		m_core->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		m_core->glEnableVertexAttribArray(2);

		m_is_reload = false;
	}

	m_world.setToIdentity();
	m_world.rotate(m_xRot / 16.0f, 1, 0, 0);
	m_world.rotate(m_yRot / 16.0f, 0, 1, 0);
	m_world.rotate(m_zRot / 16.0f, 0, 0, 1);

	m_shader->use();
	m_core->glBindVertexArray(VAOObj);
	m_projMatrixLoc = m_shader->shader_program.uniformLocation("proj_mat");
	m_modelMatrixLoc = m_shader->shader_program.uniformLocation("model_mat");
	m_viewMatrixLoc = m_shader->shader_program.uniformLocation("view_mat");
	m_aLightPosLoc = m_shader->shader_program.uniformLocation("a_light_pos");
	m_bLightPosLoc = m_shader->shader_program.uniformLocation("b_light_pos");
	m_viewPosLoc = m_shader->shader_program.uniformLocation("view_pos");

	m_shader->shader_program.setUniformValue(m_projMatrixLoc, m_proj);
	m_shader->shader_program.setUniformValue(m_modelMatrixLoc, m_world);
	m_shader->shader_program.setUniformValue(m_viewMatrixLoc, m_camera);
	m_shader->shader_program.setUniformValue(m_aLightPosLoc, m_a_light_pos);
	m_shader->shader_program.setUniformValue(m_bLightPosLoc, m_b_light_pos);
	m_shader->shader_program.setUniformValue(m_viewPosLoc, m_view_pos);
	m_core->glDrawArrays(GL_TRIANGLES, 0, m_object_3d->m_num_faces * 3);

	m_shader->shader_program.release();
}

void MainOpenGLWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_Escape:
		close();
	case Qt::Key_M:
		if (event->isAutoRepeat() == true)
		{
			// stay press
			cv::Mat cv_frame;
			m_video_cap >> cv_frame;

			dlib::array2d<dlib::bgr_pixel> dlib_img;
			dlib::assign_image(dlib_img, dlib::cv_image<dlib::bgr_pixel>(cv_frame));
			dlib::pyramid_up(dlib_img);

			std::vector<dlib::rectangle> dets = m_detector(dlib_img);
			if (dets.size() > 0)
			{
				std::cout << "detctors: " << dets[0].bottom() << std::endl;
				glm::ivec4 current_face_det = glm::ivec4(dets[0].left(), dets[0].top(), dets[0].right(), dets[0].bottom());

				current_face_det -= m_start_face_det;

				setXRotation(m_xRot + -1 * current_face_det[1]);
				setYRotation(m_yRot + -1 * current_face_det[0]);
			}

			cv::imshow("test", cv_frame);
		}
		else
		{
			// when press
			std::cout << "Face-moving Now" << std::endl;

			cv::Mat cv_frame;
			m_video_cap >> cv_frame;

			dlib::array2d<dlib::bgr_pixel> dlib_img;
			dlib::assign_image(dlib_img, dlib::cv_image<dlib::bgr_pixel>(cv_frame));
			dlib::pyramid_up(dlib_img);

			std::vector<dlib::rectangle> dets = m_detector(dlib_img);

			if (dets.size() > 0)
			{
				std::cout << "detctors: " << dets[0].bottom() << std::endl;
				m_start_face_det = glm::ivec4(dets[0].left(), dets[0].top(), dets[0].right(), dets[0].bottom());
			}

			dets.clear();
		}
	default:
		break;
	}
}

void MainOpenGLWidget::mousePressEvent(QMouseEvent *event)
{
	m_lastPos = event->pos();
}

void MainOpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
	int dx = event->x() - m_lastPos.x();
	int dy = event->y() - m_lastPos.y();

	if (event->buttons() & Qt::LeftButton)
	{
		setXRotation(m_xRot + 8 * dy);
		setYRotation(m_yRot + 8 * dx);
	}
	else if (event->buttons() & Qt::RightButton)
	{
		setXRotation(m_xRot + 8 * dy);
		setZRotation(m_zRot + 8 * dx);
	}
	else if (event->buttons() & Qt::MidButton)
	{
		m_camera.translate((GLfloat)dx / 600, -1 * (GLfloat)dy / 600, 0);
		update();
	}
	m_lastPos = event->pos();
}

void MainOpenGLWidget::wheelEvent(QWheelEvent *event)
{
	if (event->delta() > 0)
	{
		m_camera.translate(0, 0, 0.05f);
		update();
	}
	else
	{
		m_camera.translate(0, 0, -0.05f);
		update();
	}
}