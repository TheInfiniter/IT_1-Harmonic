
// HarmonicDlg.h: файл заголовка
//

#pragma once


// Диалоговое окно CHarmonicDlg
class CHarmonicDlg : public CDialogEx
{
// Создание
public:
	CHarmonicDlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HARMONIC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//Картинка
	//Обычный график
	CWnd *SignalWnd;
	CDC *SignalDC;
	CRect SignalRect;

	//График ошибки
	CWnd *ErrorWnd;
	CDC *ErrorDC;
	CRect ErrorRect;

	CPen Axis; //кисть для рисования осей
	CPen Grid; //сетки
	CPen Line; //графика
	CPen Filter; //фильтра
	CPen Level; //уровня

	double freq1, freq2, freq3; //частоты
	int arraysize; //количество отсчетов
	int leftrange, rightrange; //левая и правая границы
	double a1, a2; //коэффициенты построения ошибки
	int lenght, height; //коэффициенты фильтрации (длина и высота фильтрационного окна)
	int filter_lenght; //вводимая с клавиатуры длина фильтра
	double edge_count; //пороговая переменная
	double edge_tip; //подсказка для порога
	double level; //для рисования линии порога
	int start_pred, end_pred; //предсказанные конец и начало
	BOOL Start; //начало программы

	int element; //количество элементов в сумме шума
	double signal_en; //энергия сигнала
	double noise_en; //энергия шума
	double summ; //сумма элементов шумового сигнала
	double alpha; //нормировочный коэффициент
	double noise_percent; //процент шума

	//массивы
	double *record; //массив исходного сигнала
	double *auto_reg; //массив авторегрессии
	double *error; //массив ошибки
	double *filter; //массив фильтрации

	//коэффициенты для рисования
	double xmin_graph, xmax_graph, ymin_graph, ymax_graph, xtrans_graph, ytrans_graph; //график
	double xmin_error, xmax_error, ymin_error, ymax_error, xtrans_error, ytrans_error; //ошибка

	afx_msg void OnBnClickedDraw();
};
