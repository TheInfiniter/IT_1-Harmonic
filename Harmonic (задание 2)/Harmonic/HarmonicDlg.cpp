
// HarmonicDlg.cpp: файл реализации
//

#include "stdafx.h"
#include "Harmonic.h"
#include "HarmonicDlg.h"
#include "afxdialogex.h"
#include "Solution.h"
#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//макрос для пересчёта координат
#define DOTS_GRAPH(x, y) (xtrans_graph * ((x) - xmin_graph)), (ytrans_graph * ((y) - ymax_graph)) //график
#define DOTS_ERROR(x, y) (xtrans_error * ((x) - xmin_error)), (ytrans_error * ((y) - ymax_error)) //ошибка

// Диалоговое окно CHarmonicDlg

CHarmonicDlg::CHarmonicDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HARMONIC_DIALOG, pParent)
	, freq1(0.3)
	, freq2(0.1)
	, freq3(0.2)
	, arraysize(256)
	, leftrange(70)
	, rightrange(140)
	, edge_count(0.00001)
	, start_pred(0)
	, end_pred(0)
	, Start(FALSE)
	, element(15)
	, noise_percent(5)
	, edge_tip(0)
	, filter_lenght(20)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHarmonicDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FREQ1, freq1);
	DDX_Text(pDX, IDC_FREQ2, freq2);
	DDX_Text(pDX, IDC_ARRAY_EDIT, arraysize);
	DDX_Text(pDX, IDC_FREQ3, freq3);
	DDX_Text(pDX, IDC_LEFT, leftrange);
	DDX_Text(pDX, IDC_RIGHT, rightrange);
	DDX_Text(pDX, IDC_STARTPRED, start_pred);
	DDX_Text(pDX, IDC_ENDPRED, end_pred);
	DDX_Text(pDX, IDC_NOISEPERCENT, noise_percent);
	DDX_Text(pDX, IDC_EDGE, edge_count);
	DDX_Text(pDX, IDC_TIP, edge_tip);
	DDX_Text(pDX, IDC_FILTER, filter_lenght);
}

BEGIN_MESSAGE_MAP(CHarmonicDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_DRAW, &CHarmonicDlg::OnBnClickedDraw)
END_MESSAGE_MAP()


// Обработчики сообщений CHarmonicDlg

BOOL CHarmonicDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию

	//График
	SignalWnd = GetDlgItem(IDC_SIGNAL);
	SignalDC = SignalWnd->GetDC();
	SignalWnd->GetClientRect(&SignalRect);
	//Ошибка
	ErrorWnd = GetDlgItem(IDC_ERROR);
	ErrorDC = ErrorWnd->GetDC();
	ErrorWnd->GetClientRect(&ErrorRect);

	//График
	xmin_graph = -5.0;
	xmax_graph = arraysize;
	ymin_graph = -1.0;
	ymax_graph = 1.0;
	xtrans_graph = ((double)(SignalRect.Width()) / (xmax_graph - xmin_graph));
	ytrans_graph = -((double)(SignalRect.Height()) / (ymax_graph - ymin_graph));
	//Ошибка
	xmin_error = -5.0;
	xmax_error = arraysize;
	ymin_error = -0.2;
	ymax_error = 0.2;
	xtrans_error = ((double)(ErrorRect.Width()) / (xmax_error - xmin_error));
	ytrans_error = -((double)(ErrorRect.Height()) / (ymax_error - ymin_error));

	Axis.CreatePen(PS_SOLID, 2, RGB(0, 0, 0)); //оси: прямая линия, толщина 2 пикселя, черный цвет
	Grid.CreatePen(PS_DOT, 1, RGB(0, 0, 0)); //сетка: пунктирная линия, 1 пиксель, черный
	Line.CreatePen(PS_SOLID, 2, RGB(0, 0, 255)); //график: прямая линия, 1 пиксель, синий
	Filter.CreatePen(PS_SOLID, 2, RGB(255, 0, 0)); //фильтрованная ошибка: прямая линия, 1 пиксель, красный
	Level.CreatePen(PS_SOLID, 2, RGB(255, 0, 255)); //уровень: прямая линия, 1 пиксель, фиолетовый

	UpdateData(FALSE);
	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CHarmonicDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();

		SignalDC->FillSolidRect(SignalRect, RGB(242, 225, 241));
		ErrorDC->FillSolidRect(ErrorRect, RGB(242, 225, 241));

		xtrans_graph = ((double)(SignalRect.Width()) / (xmax_graph - xmin_graph));
		ytrans_graph = -((double)(SignalRect.Height()) / (ymax_graph - ymin_graph));
		xtrans_error = ((double)(ErrorRect.Width()) / (xmax_error - xmin_error));
		ytrans_error = -((double)(ErrorRect.Height()) / (ymax_error - ymin_error));

		//надписи
		//график
		SignalDC->SelectObject(&Axis); //выбор кисти для осей
		for (int i = 0; i <= xmax_graph; i = i + (arraysize / 8)) //засечки
		{
			SignalDC->MoveTo(DOTS_GRAPH(i, 0.02 * (ymax_graph - ymin_graph)));
			SignalDC->LineTo(DOTS_GRAPH(i, -0.02 * (ymax_graph - ymin_graph)));
		}
		for (double i = 0; i <= xmax_graph; i = i + (arraysize / 8))
		{
			CString str;
			str.Format(_T("%.1f"), i); //вытаскивание числа из i!
			SignalDC->TextOutW(DOTS_GRAPH(i - 0.02 * (ymax_graph - ymin_graph), 0 - 0.02 * (ymax_graph - ymin_graph)), str);
		}
		CString str;
		str.Format(_T("%.1f"), ymin_graph); //вытаскивание числа из i!
		SignalDC->TextOutW(DOTS_GRAPH(0.02 * (xmax_graph - xmin_graph), ymin_graph + 0.1 * (ymax_graph - ymin_graph)), str);
		str.Format(_T("%.1f"), ymax_graph); //вытаскивание числа из i!
		SignalDC->TextOutW(DOTS_GRAPH(0.02 * (xmax_graph - xmin_graph), ymax_graph), str);

		//ошибка
		ErrorDC->SelectObject(&Axis); //выбор кисти для осей
		for (int i = 0; i <= xmax_error; i = i + (arraysize / 8)) //засечки
		{
			ErrorDC->MoveTo(DOTS_ERROR(i, 0.02 * (ymax_error - ymin_error)));
			ErrorDC->LineTo(DOTS_ERROR(i, -0.02 * (ymax_error - ymin_error)));
		}
		for (double i = 0; i <= xmax_error; i = i + (arraysize / 8))
		{
			CString str;
			str.Format(_T("%.1f"), i); //вытаскивание числа из i!
			ErrorDC->TextOutW(DOTS_ERROR(i - 0.02 * (ymax_error - ymin_error), 0 - 0.02 * (ymax_error - ymin_error)), str);
		}
		str.Format(_T("%.1f"), ymin_error); //вытаскивание числа из i!
		ErrorDC->TextOutW(DOTS_ERROR(0.02 * (xmax_error - xmin_error), ymin_error + 0.1 * (ymax_error - ymin_error)), str);
		str.Format(_T("%.1f"), ymax_error); //вытаскивание числа из i!
		ErrorDC->TextOutW(DOTS_ERROR(0.02 * (xmax_error - xmin_error), ymax_error), str);

		//прорисовка осей
		//График
		//ось Х
		SignalDC->MoveTo(DOTS_GRAPH(xmin_graph, 0));
		SignalDC->LineTo(DOTS_GRAPH(xmax_graph, 0));
		//ось Y
		SignalDC->MoveTo(DOTS_GRAPH(0, ymin_graph));
		SignalDC->LineTo(DOTS_GRAPH(0, ymax_graph));

		//Ошибка
		//ось Х
		ErrorDC->MoveTo(DOTS_ERROR(xmin_error, 0));
		ErrorDC->LineTo(DOTS_ERROR(xmax_error, 0));
		//ось Y
		ErrorDC->MoveTo(DOTS_ERROR(0, ymin_error));
		ErrorDC->LineTo(DOTS_ERROR(0, ymax_error));

		//выполнение программы
		if (Start == true)
		{
			//UpdateData(TRUE);
			xmax_graph = arraysize;
			xmax_error = arraysize;
			//OnPaint();
			/*
			int leftrange = arraysize / 3;
			int rightrange = leftrange * 2;
			*/
			SignalDC->SelectObject(&Grid);
			SignalDC->MoveTo(DOTS_GRAPH(leftrange, ymin_graph));
			SignalDC->LineTo(DOTS_GRAPH(leftrange, ymax_graph));
			SignalDC->MoveTo(DOTS_GRAPH(rightrange, ymin_graph));
			SignalDC->LineTo(DOTS_GRAPH(rightrange, ymax_graph));

			/*
			ErrorDC->SelectObject(&Grid);
			ErrorDC->MoveTo(DOTS_ERROR(leftrange, ymin_error));
			ErrorDC->LineTo(DOTS_ERROR(leftrange, ymax_error));
			ErrorDC->MoveTo(DOTS_ERROR(rightrange, ymin_error));
			ErrorDC->LineTo(DOTS_ERROR(rightrange, ymax_error));
			*/

			record = new double[arraysize];
			error = new double[arraysize];
			filter = new double[arraysize];

			//заполняем массив сигнала так, чтобы шов был незаметен
			for (int i = 0; i < leftrange; i++)
			{
				record[i] = sin(freq1 * i);
			}
			for (int i = leftrange; i < rightrange; i++)
			{
				record[i] = sin(freq1 * leftrange + (i - leftrange) * freq2);
			}
			for (int i = rightrange; i < arraysize; i++)
			{
				record[i] = sin(freq1 * leftrange + (rightrange - leftrange) * freq2 + (i - rightrange) * freq3);
			}

			if (IsDlgButtonChecked(IDC_ADDNOISE))
			{
				//начинаем добавлять шум
				double *gauss_noise = new double[element];
				double *signal_array = new double[arraysize];

				for (int i = 0; i < element; i++)
				{
					gauss_noise[i] = 0;
				}

				for (int i = 0; i < arraysize; i++)
				{
					signal_array[i] = 0;
					for (int j = 0; j < element; j++)
					{
						gauss_noise[j] = rand() / (double)RAND_MAX * 2 - 1;
						signal_array[i] += gauss_noise[j];
					}
					signal_array[i] /= element;
				}

				for (int i = 0; i < arraysize; i++)
				{
					signal_en += (record[i] * record[i]);
					summ += (signal_array[i] * signal_array[i]);
				}

				noise_en = noise_percent * signal_en / 100; //энергия шума
				alpha = sqrt(noise_en / summ);

				//прибавляем шум к основному сигналу
				for (int i = 0; i < arraysize; i++)
				{
					record[i] = record[i] + alpha * signal_array[i];
				}

				delete[] gauss_noise;
				delete[] signal_array;
			}

			//определяем коэффициенты
			double a1 = -2 * cos(freq2);
			double a2 = 1;

			//заполняем массив ошибки
			for (int i = 0; i < arraysize; i++)
			{
				error[i] = 0;
			}
			for (int i = 2; i < arraysize; i++)
			{
				error[i] = record[i] + a1 * record[i - 1] + a2 * record[i - 2];
			}

			//превращаем ошибку в модуль квадрата ошибки
			for (int i = 0; i < arraysize; i++)
			{
				error[i] = abs(pow(error[i], 2));
			}

			//начинаем фильтровать ошибку
			for (int i = 0; i < arraysize; i++)
			{
				filter[i] = 0;
			}

			//lenght = 1 + filter_lenght / freq2;
			lenght = filter_lenght;

			for (int k = (lenght - 1) / 2; k < arraysize - (lenght - 1) / 2; k++)
			{
				for (int i = -(lenght - 1) / 2; i < (lenght - 1) / 2; i++)
				{
					filter[k] += error[k - i] / lenght;
				}
			}

			for (int i = 0; i < (lenght - 1) / 2; i++)
			{
				filter[i] = filter[(lenght - 1) / 2];
			}

			for (int i = arraysize - (lenght - 1) / 2; i < arraysize; i++)
			{
				filter[i] = filter[arraysize - 1 - (lenght - 1) / 2];
			}

			//начинаем предсказывать начало и конец
			int step1 = 1; //начало
			do
			{
				start_pred = step1;
				step1++;
			} while (filter[step1] > edge_count);
			int step2 = arraysize - 1; //конец
			do
			{
				end_pred = step2;
				step2--;
			} while (filter[step2] > edge_count);

			/*
			//пробуем подсказать, какой порог выбрать
			edge_tip = (1 + 3) / freq2;
			level = edge_count * ymax_error / 100;
			*/

			//работаем с масштабом окна ошибки, чтобы графики уместились в окне
			ymin_graph = 0;
			ymax_graph = 0;
			ymax_error = 0;
			ymin_error = -0.02;
			for (int i = 0; i < arraysize; i++)
			{
				if (error[i] > ymax_error) ymax_error = error[i];
				if (error[i] < ymin_error) ymin_error = error[i];
				if (record[i] < ymin_graph) ymin_graph = record[i];
				if (record[i] > ymax_graph) ymax_graph = record[i];
			}
			ymin_error = -ymax_error / 2;
			SignalDC->FillSolidRect(SignalRect, RGB(242, 225, 241));
			ErrorDC->FillSolidRect(ErrorRect, RGB(242, 225, 241));

			xtrans_graph = ((double)(SignalRect.Width()) / (xmax_graph - xmin_graph));
			ytrans_graph = -((double)(SignalRect.Height()) / (ymax_graph - ymin_graph));
			xtrans_error = ((double)(ErrorRect.Width()) / (xmax_error - xmin_error));
			ytrans_error = -((double)(ErrorRect.Height()) / (ymax_error - ymin_error));

			//надписи
			//график
			SignalDC->SelectObject(&Axis); //выбор кисти для осей
			for (int i = 0; i <= xmax_graph; i = i + (arraysize / 8)) //засечки
			{
				SignalDC->MoveTo(DOTS_GRAPH(i, 0.02 * (ymax_graph - ymin_graph)));
				SignalDC->LineTo(DOTS_GRAPH(i, -0.02 * (ymax_graph - ymin_graph)));
			}
			for (double i = 0; i <= xmax_graph; i = i + (arraysize / 8))
			{
				CString str;
				str.Format(_T("%.1f"), i); //вытаскивание числа из i!
				SignalDC->TextOutW(DOTS_GRAPH(i - 0.02 * (ymax_graph - ymin_graph), 0 - 0.02 * (ymax_graph - ymin_graph)), str);
			}
			CString str;
			str.Format(_T("%.1f"), ymin_graph); //вытаскивание числа из i!
			SignalDC->TextOutW(DOTS_GRAPH(0.02 * (xmax_graph - xmin_graph), ymin_graph + 0.1 * (ymax_graph - ymin_graph)), str);
			str.Format(_T("%.1f"), ymax_graph); //вытаскивание числа из i!
			SignalDC->TextOutW(DOTS_GRAPH(0.02 * (xmax_graph - xmin_graph), ymax_graph), str);

			//ошибка
			ErrorDC->SelectObject(&Axis); //выбор кисти для осей
			for (int i = 0; i <= xmax_error; i = i + (arraysize / 8)) //засечки
			{
				ErrorDC->MoveTo(DOTS_ERROR(i, 0.02 * (ymax_error - ymin_error)));
				ErrorDC->LineTo(DOTS_ERROR(i, -0.02 * (ymax_error - ymin_error)));
			}
			for (double i = 0; i <= xmax_error; i = i + (arraysize / 8))
			{
				CString str;
				str.Format(_T("%.1f"), i); //вытаскивание числа из i!
				ErrorDC->TextOutW(DOTS_ERROR(i - 0.02 * (ymax_error - ymin_error), 0 - 0.02 * (ymax_error - ymin_error)), str);
			}
			str.Format(_T("%.1f"), ymin_error); //вытаскивание числа из i!
			ErrorDC->TextOutW(DOTS_ERROR(0.02 * (xmax_error - xmin_error), ymin_error + 0.1 * (ymax_error - ymin_error)), str);
			str.Format(_T("%.1f"), ymax_error); //вытаскивание числа из i!
			ErrorDC->TextOutW(DOTS_ERROR(0.02 * (xmax_error - xmin_error), ymax_error), str);

			//прорисовка осей
			//График
			//ось Х
			SignalDC->MoveTo(DOTS_GRAPH(xmin_graph, 0));
			SignalDC->LineTo(DOTS_GRAPH(xmax_graph, 0));
			//ось Y
			SignalDC->MoveTo(DOTS_GRAPH(0, ymin_graph));
			SignalDC->LineTo(DOTS_GRAPH(0, ymax_graph));

			//Ошибка
			//ось Х
			ErrorDC->MoveTo(DOTS_ERROR(xmin_error, 0));
			ErrorDC->LineTo(DOTS_ERROR(xmax_error, 0));
			//ось Y
			ErrorDC->MoveTo(DOTS_ERROR(0, ymin_error));
			ErrorDC->LineTo(DOTS_ERROR(0, ymax_error));
			//OnPaint();

			//отмечаем предсказанные границы на графике ошибки
			ErrorDC->SelectObject(&Grid);
			ErrorDC->MoveTo(DOTS_ERROR(start_pred, ymin_error));
			ErrorDC->LineTo(DOTS_ERROR(start_pred, ymax_error));
			ErrorDC->MoveTo(DOTS_ERROR(end_pred, ymin_error));
			ErrorDC->LineTo(DOTS_ERROR(end_pred, ymax_error));

			//рисование графиков
			SignalDC->SelectObject(&Line);
			ErrorDC->SelectObject(&Line);
			SignalDC->MoveTo(DOTS_GRAPH(0, record[0]));
			ErrorDC->MoveTo(DOTS_ERROR(0, error[0]));
			for (int i = 0; i < arraysize; i++)
			{
				SignalDC->LineTo(DOTS_GRAPH(i, record[i]));
				ErrorDC->LineTo(DOTS_ERROR(i, error[i]));
			}

			//рисуем фильтрофанную ошибку
			ErrorDC->SelectObject(&Filter);
			ErrorDC->MoveTo(DOTS_ERROR(0, filter[0]));
			for (int i = 0; i < arraysize; i++)
			{
				ErrorDC->LineTo(DOTS_ERROR(i, filter[i]));
			}
			/*
			ErrorDC->SelectObject(&Level);
			ErrorDC->MoveTo(DOTS_ERROR(0, level));
			ErrorDC->LineTo(DOTS_ERROR(xmax_error, level));
			*/
			delete[] record;
			delete[] error;
			delete[] filter;
		}
		
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CHarmonicDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CHarmonicDlg::OnBnClickedDraw()
{
	// TODO: добавьте свой код обработчика уведомлений
	Start = true;
	UpdateData(TRUE);
	OnPaint();
	//Invalidate();
	UpdateData(FALSE);
}

